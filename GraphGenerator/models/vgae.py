import scipy.sparse as sp
import torch, copy, math
from torch.nn.modules.module import Module
from torch.nn.parameter import Parameter
from torch.autograd import Variable
import torch.nn as nn
import torch.nn.functional as F
import numpy as np


def coo_to_csp(sp_coo):
    num = sp_coo.shape[0]
    row = sp_coo.row
    col = sp_coo.col
    sp_tensor = torch.sparse.FloatTensor(torch.LongTensor(np.stack([row, col])),
                                         torch.tensor(sp_coo.data),
                                         torch.Size([num, num]))
    return sp_tensor


def sp_normalize(adj_def, device='cpu'):
    """
    :param adj: scipy.sparse.coo_matrix
    :param device: default as cpu
    :return: normalized_adj:
    """
    adj_ = sp.coo_matrix(adj_def)
    adj_ = adj_ + sp.coo_matrix(sp.eye(adj_def.shape[0]), dtype=np.float32)
    rowsum = np.array(adj_.sum(axis=1)).reshape(-1)
    norm_unit = np.float_power(rowsum, -0.5).astype(np.float32)
    degree_mat_inv_sqrt = sp.diags(norm_unit)
    degree_mat_sqrt = copy.copy(degree_mat_inv_sqrt)
    # degree_mat_sqrt = degree_mat_inv_sqrt.to_dense()
    support = adj_.__matmul__(degree_mat_sqrt)
    # support = coo_to_csp(support.tocoo())
    # degree_mat_inv_sqrt = coo_to_csp(degree_mat_inv_sqrt.tocoo())
    adj_normalized = degree_mat_inv_sqrt.__matmul__(support)
    adj_normalized = coo_to_csp(adj_normalized.tocoo())
    return adj_normalized, rowsum.astype(int)


class GraphConvolution(Module):
    """
    Simple GCN layer, similar to https://arxiv.org/abs/1609.02907
    """

    def __init__(self, in_features, out_features, bias=True, act=lambda x: x):
        super(GraphConvolution, self).__init__()
        self.in_features = in_features
        self.out_features = out_features
        self.weight = Parameter(torch.FloatTensor(in_features, out_features))
        self.act = act
        if bias:
            self.bias = Parameter(torch.FloatTensor(out_features))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

    def reset_parameters(self):
        stdv = 1. / math.sqrt(self.weight.size(1))
        self.weight.data.uniform_(-stdv, stdv)
        if self.bias is not None:
            self.bias.data.uniform_(-stdv, stdv)

    def forward(self, input, adj):
        support = torch.mm(input, self.weight)
        output = torch.mm(adj, support)
        if self.bias is not None:
            output = output + self.bias
        return self.act(output)

    def __repr__(self):
        return self.__class__.__name__ + ' (' \
               + str(self.in_features) + ' -> ' \
               + str(self.out_features) + ')'


class GAE(nn.Module):
    def __init__(self, input_size, emb_size, hidden_size, act=lambda x: x, layers=2):
        super(GAE, self).__init__()
        self.encoder = GraphConvolution(input_size, hidden_size, act=act)
        self.medium = nn.ModuleList([GraphConvolution(hidden_size, hidden_size, act=act) for i in range(layers-2)])
        self.mean = GraphConvolution(hidden_size, emb_size, act=lambda x: x)

    def forward(self, adj, x=None, device='cuda:0'):
        if x is None:
            x = Variable(torch.rand(adj.shape[0], self.input_size, dtype=torch.float32)).to(device)
        support = self.encoder(x, adj)
        for m in self.medium:
            support = m(support, adj)
        support = self.mean(support, adj)
        score = torch.mm(support, support.T)
        return score


def train(sp_adj, feature, config, model, optimizer):
    norm = sp_adj.shape[0] * sp_adj.shape[0] / float((sp_adj.shape[0] * sp_adj.shape[0] - sp_adj.sum()) * 2)
    pos_weight = float(sp_adj.shape[0] * sp_adj.shape[0] - sp_adj.sum()) / sp_adj.sum()
    adj_def = torch.from_numpy(sp_adj.toarray()).to(config.device)
    adj_normalized = sp_normalize(sp_adj, config.device)
    adj_normalized = Variable(adj_normalized).to(config.device)
    for epoch in range(config.max_epochs):
        adj_score = model(adj_normalized, feature, device=config.device)
        train_loss = norm * F.binary_cross_entropy_with_logits(adj_score, adj_def,
                                                               pos_weight=pos_weight)
        optimizer.zero_grad()
        train_loss.backward()
        optimizer.step()
    return 0
