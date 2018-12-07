#ifndef EDGE_H
#define EDGE_H


class Edge{
    int to;
	int dist;
public:
	Edge(int t, int d);
	int getTo() const;
	int getDist() const;
};

#endif // EDGE_H
