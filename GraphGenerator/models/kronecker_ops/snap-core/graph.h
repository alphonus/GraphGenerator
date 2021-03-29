class TUNGraph;
class TBPGraph;
typedef TPt<TUNGraph> PUNGraph;
typedef TPt<TBPGraph> PBPGraph;
class TNGraph;
class TNEGraph;
typedef TPt<TNGraph> PNGraph;
typedef TPt<TNEGraph> PNEGraph;
class TUNGraph {
public:
  typedef TUNGraph TNet;
  typedef TPt<TUNGraph> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV NIdV;
  public:
    TNode() : Id(-1), NIdV() { }
    TNode(const int& NId) : Id(NId), NIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV) { }
    TNode(TSIn& SIn) : Id(SIn), NIdV(SIn) { }
    void LoadShM(TShMIn& ShMIn) {
      Id = TInt(ShMIn);
      NIdV.LoadShM(ShMIn);
    }
    void Save(TSOut& SOut) const { Id.Save(SOut); NIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return NIdV.Len(); }
    int GetInDeg() const { return GetDeg(); }
    int GetOutDeg() const { return GetDeg(); }
    int GetInNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetOutNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetNbrNId(const int& NodeN) const { return NIdV[NodeN]; }
    bool IsNbrNId(const int& NId) const { return NIdV.SearchBin(NId)!=-1; }
    bool IsInNId(const int& NId) const { return IsNbrNId(NId); }
    bool IsOutNId(const int& NId) const { return IsNbrNId(NId); }
    void PackOutNIdV() { NIdV.Pack(); }
    void PackNIdV() { NIdV.Pack(); }
    void SortNIdV() { NIdV.Sort();}
    friend class TUNGraph;
    friend class TUNGraphMtx;
  };

  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }

    TNodeI& operator++ (int) { NodeHI++; return *this; }

    TNodeI& operator-- (int) { NodeHI--; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    int GetId() const { return NodeHI.GetDat().GetId(); }

    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }

    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }

    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }

    void SortNIdV() { NodeHI.GetDat().SortNIdV(); }

    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }

    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }

    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }

    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }

    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }

    bool IsNbrNId(const int& NId) const { return NodeHI.GetDat().IsNbrNId(NId); }
    friend class TUNGraph;
  };

  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; } return *this; }

    TEdgeI& operator++ (int) { do { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++; while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } } } while (CurNode < EndNode && GetSrcNId()>GetDstNId()); return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }

    int GetId() const { return -1; }

    int GetSrcNId() const { return CurNode.GetId(); }

    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TUNGraph;
  };
private:
  TCRef CRef;
  TInt MxNId, NEdges;
  THash<TInt, TNode> NodeH;
private:
  class TLoadTNodeInitializer {
  public:
    TLoadTNodeInitializer() {}
    void operator() (TNode* Node, TShMIn& ShMIn) { Node->LoadShM(ShMIn);}
  };
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  void LoadGraphShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    NEdges = TInt(ShMIn);
    TLoadTNodeInitializer Fn;
    NodeH.LoadShM(ShMIn, Fn);
  }
public:
  TUNGraph() : CRef(), MxNId(0), NEdges(0), NodeH() { }

  explicit TUNGraph(const int& Nodes, const int& Edges) : MxNId(0), NEdges(0) { Reserve(Nodes, Edges); }
  TUNGraph(const TUNGraph& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), NodeH(Graph.NodeH) { }

  TUNGraph(TSIn& SIn) : MxNId(SIn), NEdges(SIn), NodeH(SIn) { }

  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeH.Save(SOut); }

  static PUNGraph New() { return new TUNGraph(); }

  static PUNGraph New(const int& Nodes, const int& Edges) { return new TUNGraph(Nodes, Edges); }

  static PUNGraph Load(TSIn& SIn) { return PUNGraph(new TUNGraph(SIn)); }

  static PUNGraph LoadShM(TShMIn& ShMIn) {
    TUNGraph* Graph = new TUNGraph();
    Graph->LoadGraphShM(ShMIn);
    return PUNGraph(Graph);
  }
  bool HasFlag(const TGraphFlag& Flag) const;
  TUNGraph& operator = (const TUNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; NodeH=Graph.NodeH; } return *this; }
  

  int GetNodes() const { return NodeH.Len(); }

  int AddNode(int NId = -1);

  int AddNodeUnchecked(int NId = -1);

  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }

  int AddNode(const int& NId, const TIntV& NbrNIdV);

  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId);

  void DelNode(const int& NId);

  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }

  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }

  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }

  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }

  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }

  int GetMxNId() const { return MxNId; }

  int GetEdges() const;

  int AddEdge(const int& SrcNId, const int& DstNId);

  int AddEdge(const int& SrcNId, const int& DstNId, const int& EId) { return AddEdge(SrcNId, DstNId); }

  int AddEdgeUnchecked(const int& SrcNId, const int& DstNId);

  int AddEdge2(const int& SrcNId, const int& DstNId);

  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }

  void DelEdge(const int& SrcNId, const int& DstNId);

  bool IsEdge(const int& SrcNId, const int& DstNId) const;

  bool IsEdge(const int& EId) const { return false; }

  TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); if (GetNodes() != 0 && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { EI++; } return EI; }

  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }

  TEdgeI GetEI(const int& EId) const;

  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }

  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }

  void GetNIdV(TIntV& NIdV) const;

  bool Empty() const { return GetNodes()==0; }

  void Clr() { MxNId=0; NEdges=0; NodeH.Clr(); }

  void SortNodeAdjV() { for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NI.SortNIdV();} }

  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) NodeH.Gen(Nodes/2); }

  void ReserveNIdDeg(const int& NId, const int& Deg) { GetNode(NId).NIdV.Reserve(Deg); }

  void Defrag(const bool& OnlyNodeLinks=false);

  bool IsOk(const bool& ThrowExcept=true) const;

  void Dump(FILE *OutF=stdout) const;

  static PUNGraph GetSmallGraph();
  friend class TUNGraphMtx;
  friend class TPt<TUNGraph>;
};
class TNGraph {
public:
  typedef TNGraph TNet;
  typedef TPt<TNGraph> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InNIdV, OutNIdV;
  public:
    TNode() : Id(-1), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InNIdV.Save(SOut); OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InNIdV.Len(); }
    int GetOutDeg() const { return OutNIdV.Len(); }
    int GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
    int GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
    int GetNbrNId(const int& NodeN) const { return NodeN<GetOutDeg()?GetOutNId(NodeN):GetInNId(NodeN-GetOutDeg()); }
    bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId) != -1; }
    bool IsOutNId(const int& NId) const { return OutNIdV.SearchBin(NId) != -1; }
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void PackOutNIdV() { OutNIdV.Pack(); }
    void PackNIdV() { InNIdV.Pack(); }
    void SortNIdV() { InNIdV.Sort(); OutNIdV.Sort();}
    void LoadShM(TShMIn& ShMIn) {
      Id = TInt(ShMIn);
      InNIdV.LoadShM(ShMIn);
      OutNIdV.LoadShM(ShMIn);
    }
    friend class TNGraph;
    friend class TNGraphMtx;
  };

  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }

    TNodeI& operator++ (int) { NodeHI++; return *this; }

    TNodeI& operator-- (int) { NodeHI--; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    int GetId() const { return NodeHI.GetDat().GetId(); }

    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }

    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }

    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }

    void SortNIdV() { NodeHI.GetDat().SortNIdV(); }

    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }

    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }

    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }

    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }

    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }

    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TNGraph;
  };

  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; }  return *this; }

    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }

    int GetId() const { return -1; }

    int GetSrcNId() const { return CurNode.GetId(); }

    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TNGraph;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;
private:
  class TLoadTNodeInitializer {
  public:
    TLoadTNodeInitializer() {}
    void operator() (TNode* Node, TShMIn& ShMIn) {Node->LoadShM(ShMIn);}
  };
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  void LoadGraphShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    TLoadTNodeInitializer Fn;
    NodeH.LoadShM(ShMIn, Fn);
  }
public:
  TNGraph() : CRef(), MxNId(0), NodeH() { }

  explicit TNGraph(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNGraph(const TNGraph& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }

  TNGraph(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }

  void Save(TSOut& SOut) const { MxNId.Save(SOut); NodeH.Save(SOut); }

  static PNGraph New() { return new TNGraph(); }

  static PNGraph New(const int& Nodes, const int& Edges) { return new TNGraph(Nodes, Edges); }

  static PNGraph Load(TSIn& SIn) { return PNGraph(new TNGraph(SIn)); }

  static PNGraph LoadShM(TShMIn& ShMIn) {
    TNGraph* Graph = new TNGraph();
    Graph->LoadGraphShM(ShMIn);
    return PNGraph(Graph);
  }

  bool HasFlag(const TGraphFlag& Flag) const;
  TNGraph& operator = (const TNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NodeH=Graph.NodeH; }  return *this; }
  

  int GetNodes() const { return NodeH.Len(); }

  int AddNode(int NId = -1);

  int AddNodeUnchecked(int NId = -1);

  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }

  int AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV);

  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId);

  void DelNode(const int& NId);

  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }

  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }

  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }

  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }

  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }



  int GetMxNId() const { return MxNId; }

  int GetEdges() const;

  int AddEdge(const int& SrcNId, const int& DstNId);

  int AddEdge(const int& SrcNId, const int& DstNId, const int& EId) { return AddEdge(SrcNId, DstNId); }

  int AddEdgeUnchecked(const int& SrcNId, const int& DstNId);

  int AddEdge2(const int& SrcNId, const int& DstNId);

  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }

  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);

  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;

  bool IsEdge(const int& EId) const { return false; }

  TEdgeI BegEI() const { TNodeI NI=BegNI(); while(NI<EndNI() && NI.GetOutDeg()==0){NI++;} return TEdgeI(NI, EndNI()); }

  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }

  TEdgeI GetEI(const int& EId) const;

  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }

  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }

  void GetNIdV(TIntV& NIdV) const;

  bool Empty() const { return GetNodes()==0; }

  void Clr() { MxNId=0; NodeH.Clr(); }

  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }

  void ReserveNIdInDeg(const int& NId, const int& InDeg) { GetNode(NId).InNIdV.Reserve(InDeg); }

  void ReserveNIdOutDeg(const int& NId, const int& OutDeg) { GetNode(NId).OutNIdV.Reserve(OutDeg); }

  void SortNodeAdjV() { for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NI.SortNIdV();} }

  void Defrag(const bool& OnlyNodeLinks=false);

  bool IsOk(const bool& ThrowExcept=true) const;

  void Dump(FILE *OutF=stdout) const;

  static PNGraph GetSmallGraph();
  friend class TPt<TNGraph>;
  friend class TNGraphMtx;
};
namespace TSnap {
template <> struct IsDirected<TNGraph> { enum { Val = 1 }; };
}
class TNEGraph {
public:
  typedef TNEGraph TNet;
  typedef TPt<TNEGraph> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
  public:
    TNode() : Id(-1), InEIdV(), OutEIdV() { }
    TNode(const int& NId) : Id(NId), InEIdV(), OutEIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InEIdV(SIn), OutEIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
    int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
    friend class TNEGraph;
  };
  class TEdge {
  private:
    TInt Id, SrcNId, DstNId;
  public:
    TEdge() : Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId) : Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    friend class TNEGraph;
  };

  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    const TNEGraph *Graph;
  public:
    TNodeI() : NodeHI(), Graph(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNEGraph* GraphPt) : NodeHI(NodeHIter), Graph(GraphPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Graph(NodeI.Graph) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }

    TNodeI& operator++ (int) { NodeHI++; return *this; }

    TNodeI& operator-- (int) { NodeHI--; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    int GetId() const { return NodeHI.GetDat().GetId(); }

    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }

    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }

    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }

    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }

    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }

    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(NodeHI.GetDat().GetNbrEId(EdgeN));
      return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }

    bool IsInNId(const int& NId) const;

    bool IsOutNId(const int& NId) const;

    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }


    int GetInEId(const int& EdgeN) const { return NodeHI.GetDat().GetInEId(EdgeN); }

    int GetOutEId(const int& EdgeN) const { return NodeHI.GetDat().GetOutEId(EdgeN); }

    int GetNbrEId(const int& EdgeN) const { return NodeHI.GetDat().GetNbrEId(EdgeN); }

    bool IsInEId(const int& EId) const { return NodeHI.GetDat().IsInEId(EId); }

    bool IsOutEId(const int& EId) const { return NodeHI.GetDat().IsOutEId(EId); }

    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    friend class TNEGraph;
  };

  class TEdgeI {
  private:
    typedef THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TNEGraph *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNEGraph *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }

    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }

    int GetId() const { return EdgeHI.GetDat().GetId(); }

    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }

    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    friend class TNEGraph;
  };
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
private:
  TCRef CRef;
  TInt MxNId, MxEId;
  THash<TInt, TNode> NodeH;
  THash<TInt, TEdge> EdgeH;
public:
  TNEGraph() : CRef(), MxNId(0), MxEId(0) { }

  explicit TNEGraph(const int& Nodes, const int& Edges) : CRef(), MxNId(0), MxEId(0) { Reserve(Nodes, Edges); }
  TNEGraph(const TNEGraph& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId), NodeH(Graph.NodeH), EdgeH(Graph.EdgeH) { }

  TNEGraph(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn) { }

  void Save(TSOut& SOut) const { MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut); }

  static PNEGraph New() { return PNEGraph(new TNEGraph()); }

  static PNEGraph New(const int& Nodes, const int& Edges) { return PNEGraph(new TNEGraph(Nodes, Edges)); }

  static PNEGraph Load(TSIn& SIn) { return PNEGraph(new TNEGraph(SIn)); }

  bool HasFlag(const TGraphFlag& Flag) const;
  TNEGraph& operator = (const TNEGraph& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }  return *this; }

  int GetNodes() const { return NodeH.Len(); }

  int AddNode(int NId = -1);

  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }

  void DelNode(const int& NId);

  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }

  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }

  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }

  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }

  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }

  int GetMxNId() const { return MxNId; }

  int GetEdges() const { return EdgeH.Len(); }

  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);

  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }

  void DelEdge(const int& EId);

  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);

  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }

  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }

  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;

  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }

  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }

  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }

  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }

  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }

  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }

  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }

  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }

  void GetNIdV(TIntV& NIdV) const;

  void GetEIdV(TIntV& EIdV) const;

  bool Empty() const { return GetNodes()==0; }

  void Clr() { MxNId=0; MxEId=0; NodeH.Clr(); EdgeH.Clr(); }

  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes/2); } if (Edges>0) { EdgeH.Gen(Edges/2); } }

  void Defrag(const bool& OnlyNodeLinks=false);

  bool IsOk(const bool& ThrowExcept=true) const;

  void Dump(FILE *OutF=stdout) const;

  static PNEGraph GetSmallGraph();
  friend class TPt<TNEGraph>;
};
namespace TSnap {
template <> struct IsMultiGraph<TNEGraph> { enum { Val = 1 }; };
template <> struct IsDirected<TNEGraph> { enum { Val = 1 }; };
}
class TBPGraph {
public:
  typedef TBPGraph TNet;
  typedef TPt<TBPGraph> PNet;
  typedef enum { bgsUndef, bgsLeft, bgsRight, bgsBoth } TNodeTy;
public:
  class TNode {
  private:
    TInt Id;
    TIntV NIdV;
    TNodeTy NodeTy;
  public:
    TNode() : Id(-1), NIdV(), NodeTy(bgsUndef) { }
    TNode(const int& NId) : Id(NId), NIdV(), NodeTy(true?bgsLeft:bgsRight) { }
    TNode(const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV), NodeTy(Node.NodeTy) { }
    TNode(TSIn& SIn) : Id(SIn), NIdV(SIn), NodeTy(bgsUndef) { TInt Ty(SIn); NodeTy=(TNodeTy)Ty.Val; }
    void Save(TSOut& SOut) const { Id.Save(SOut); NIdV.Save(SOut); TInt(NodeTy).Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return NIdV.Len(); }
    int GetInDeg() const { return GetDeg(); }
    int GetOutDeg() const { return GetDeg(); }
    int GetInNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetOutNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetNbrNId(const int& NodeN) const { return NIdV[NodeN]; }
    bool IsNbrNId(const int& NId) const { return NIdV.SearchBin(NId)!=-1; }
    bool IsInNId(const int& NId) const { return IsNbrNId(NId); }
    bool IsOutNId(const int& NId) const { return IsNbrNId(NId); }
    void PackOutNIdV() { NIdV.Pack(); }
    void PackNIdV() { NIdV.Pack(); }
    friend class TBPGraph;
  };

  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter LeftHI, RightHI;
  private:
    inline THashIter HI() const { return ! LeftHI.IsEnd()?LeftHI:RightHI; }
  public:
    TNodeI() : LeftHI(), RightHI() { }
    TNodeI(const THashIter& LeftHIter, const THashIter& RightHIter) : LeftHI(LeftHIter), RightHI(RightHIter) { }
    TNodeI(const TNodeI& NodeI) : LeftHI(NodeI.LeftHI), RightHI(NodeI.RightHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { LeftHI = NodeI.LeftHI; RightHI=NodeI.RightHI; return *this; }

    TNodeI& operator++ (int) { 
      if (! LeftHI.IsEnd()) { 
        LeftHI++; } 
      else if (! RightHI.IsEnd()) { 
        RightHI++; } 
      return *this; }
    bool operator < (const TNodeI& NodeI) const { return LeftHI < NodeI.LeftHI || (LeftHI==NodeI.LeftHI && RightHI < NodeI.RightHI); }
    bool operator == (const TNodeI& NodeI) const { return LeftHI==NodeI.LeftHI && RightHI==NodeI.RightHI; }

    int GetId() const { return HI().GetDat().GetId(); }

    bool IsLeft() const { return ! LeftHI.IsEnd(); }

    bool IsRight() const { return ! IsLeft(); }

    int GetDeg() const { return HI().GetDat().GetDeg(); }

    int GetInDeg() const { return HI().GetDat().GetInDeg(); }

    int GetOutDeg() const { return HI().GetDat().GetOutDeg(); }

    int GetInNId(const int& NodeN) const { return HI().GetDat().GetInNId(NodeN); }

    int GetOutNId(const int& NodeN) const { return HI().GetDat().GetOutNId(NodeN); }

    int GetNbrNId(const int& NodeN) const { return HI().GetDat().GetNbrNId(NodeN); }

    bool IsInNId(const int& NId) const { return HI().GetDat().IsInNId(NId); }

    bool IsOutNId(const int& NId) const { return HI().GetDat().IsOutNId(NId); }

    bool IsNbrNId(const int& NId) const { return HI().GetDat().IsNbrNId(NId); }
    friend class TBPGraph;
  };

  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; }  return *this; }

    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }

    int GetId() const { return -1; }

    int GetSrcNId() const { return CurNode.GetId(); }

    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }

    int GetLNId() const { return GetSrcNId(); }

    int GetRNId() const { return GetDstNId(); }
    friend class TBPGraph;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> LeftH;
  THash<TInt, TNode> RightH;
private:


public:
  TBPGraph() : CRef(), MxNId(0), LeftH(), RightH() { }

  explicit TBPGraph(const int& Nodes, const int& Edges) : MxNId(0) { }
  TBPGraph(const TBPGraph& BPGraph) : MxNId(BPGraph.MxNId), LeftH(BPGraph.LeftH), RightH(BPGraph.RightH) { }

  TBPGraph(TSIn& SIn) : MxNId(SIn), LeftH(SIn), RightH(SIn) { }

  void Save(TSOut& SOut) const { MxNId.Save(SOut); LeftH.Save(SOut); RightH.Save(SOut); }

  static PBPGraph New() { return new TBPGraph(); }

  static PBPGraph New(const int& Nodes, const int& Edges) { return new TBPGraph(Nodes, Edges); }

  static PBPGraph Load(TSIn& SIn) { return PBPGraph(new TBPGraph(SIn)); }

  bool HasFlag(const TGraphFlag& Flag) const;
  TBPGraph& operator = (const TBPGraph& BPGraph) {
    if (this!=&BPGraph) { MxNId=BPGraph.MxNId; LeftH=BPGraph.LeftH; RightH=BPGraph.RightH; }  return *this; }
  

  int GetNodes() const { return GetLNodes() + GetRNodes(); }

  int GetLNodes() const { return LeftH.Len(); }

  int GetRNodes() const { return RightH.Len(); }

  int AddNode(int NId = -1, const bool& LeftNode=true);

  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.IsLeft()); }

  void DelNode(const int& NId);

  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }

  bool IsNode(const int& NId) const { return IsLNode(NId) || IsRNode(NId); }

  bool IsLNode(const int& NId) const { return LeftH.IsKey(NId); }

  bool IsRNode(const int& NId) const { return RightH.IsKey(NId); }

  int GetMxNId() const { return MxNId; }
    

  TNodeI BegNI() const { return TNodeI(LeftH.BegI(), RightH.BegI()); }

  TNodeI EndNI() const { return TNodeI(LeftH.EndI(), RightH.EndI()); }

  TNodeI GetNI(const int& NId) const { return IsLNode(NId) ? TNodeI(LeftH.GetI(NId), RightH.EndI()) : TNodeI(LeftH.EndI(), RightH.GetI(NId)); }

  TNodeI BegLNI() const { return TNodeI(LeftH.BegI(), RightH.EndI()); }

  TNodeI EndLNI() const { return EndNI(); }

  TNodeI BegRNI() const { return TNodeI(LeftH.EndI(), RightH.BegI()); }

  TNodeI EndRNI() const { return EndNI(); }

  int GetEdges() const;

  int AddEdge(const int& LeftNId, const int& RightNId);

  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }

  void DelEdge(const int& LeftNId, const int& RightNId);

  bool IsEdge(const int& LeftNId, const int& RightNId) const;

  TEdgeI BegEI() const { TNodeI NI=BegLNI(); while (NI<EndLNI() && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { NI++; } return TEdgeI(NI, EndLNI()); }

  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }

  TEdgeI GetEI(const int& EId) const;

  TEdgeI GetEI(const int& LeftNId, const int& RightNId) const;
    

  int GetRndNId(TRnd& Rnd=TInt::Rnd);

  int GetRndLNId(TRnd& Rnd=TInt::Rnd);

  int GetRndRNId(TRnd& Rnd=TInt::Rnd);

  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }

  void GetNIdV(TIntV& NIdV) const;

  void GetLNIdV(TIntV& NIdV) const;

  void GetRNIdV(TIntV& NIdV) const;

  bool Empty() const { return GetNodes()==0; }

  void Clr() { MxNId=0; LeftH.Clr(); RightH.Clr(); }

  void Reserve(const int& Nodes, const int& Edges);

  void Defrag(const bool& OnlyNodeLinks=false);

  bool IsOk(const bool& ThrowExcept=true) const;

  void Dump(FILE *OutF=stdout) const;

  static PBPGraph GetSmallGraph();
  friend class TPt<TBPGraph>;
};
namespace TSnap {
template <> struct IsBipart<TBPGraph> { enum { Val = 1 }; };
}
