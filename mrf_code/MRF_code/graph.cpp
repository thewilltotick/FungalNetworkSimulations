// File: graph.cpp
// -- simple graph handling source file
//-----------------------------------------------------------------------------
// Community detection
// Based on the article "Fast unfolding of community hierarchies in large networks"
// Copyright (C) 2008 V. Blondel, J.-L. Guillaume, R. Lambiotte, E. Lefebvre
//
// This program must not be distributed without agreement of the above mentionned authors.
//-----------------------------------------------------------------------------
// Author   : E. Lefebvre, adapted by J.-L. Guillaume
// Email    : jean-loup.guillaume@lip6.fr
// Location : Paris, France
// Time	    : February 2008
//-----------------------------------------------------------------------------
// see readme.txt for more details

#include "graph.h"

using namespace std;

Graph::Graph(char *filename, int type) {
  ifstream finput;
  finput.open(filename,fstream::in);

  int nb_links=0;

  while (!finput.eof()) {
    if (nb_links%10000000==0) {cerr << "."; fflush(stderr);}	//flush(?) when the number of links equals 10 million

    unsigned int src, dest;
    double weight=1;

    if (type==WEIGHTED)
      finput >> src >> dest >> weight;	//reads in source node, destination node and weight as integers
    else
      finput >> src >> dest;
    
    if (finput) {
      if (links.size()<=max(src,dest)+1)
        links.resize(max(src,dest)+1);	//resizes the links vector to the maximum node number
      
      links[src].push_back(make_pair(dest,weight));	//add a link to source (the vector links[src] stores all links for node src)
      links[dest].push_back(make_pair(src,weight));	//add the same link to destination
      nb_links++;
    }
  }

  finput.close();
}

void
Graph::renumber(int type) {	//removes isolated nodes
  vector<int> linked(links.size(),-1);	//declare a vector the size of links (i.e. the number of nodes) with every element having a value of -1
  vector<int> renum(links.size(),-1);	//declare a second vector as above
  int nb=0;
  
  for (unsigned int i=0 ; i<links.size() ; i++) {
    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      linked[i]=1;
      linked[links[i][j].first]=1;
    }
  }
  
  for (unsigned int i=0 ; i<links.size() ; i++) {
    if (linked[i]==1)
      renum[i]=nb++;
  }

  for (unsigned int i=0 ; i<links.size() ; i++) {
    if (linked[i]==1) {
      for (unsigned int j=0 ; j<links[i].size() ; j++) {
	links[i][j].first = renum[links[i][j].first];
      }
      links[renum[i]]=links[i];
    }
  }
  links.resize(nb);
}

void
Graph::clean(int type) {
  for (unsigned int i=0 ; i<links.size() ; i++) {
    if (i%10000000==0) fprintf(stderr,".");fflush(stderr);      
    map<int,double> m;
    map<int,double>::iterator it;

    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      it = m.find(links[i][j].first);
      if (it==m.end())
	m.insert(make_pair(links[i][j].first, links[i][j].second));
      else if (type==WEIGHTED)
      	it->second+=links[i][j].second;
    }

    vector<pair<int,double> > v;
    for (it = m.begin() ; it!=m.end() ; it++)
      v.push_back(*it);
    links[i].clear();
    links[i]=v;
  }
}

void
Graph::display(int type) {
  cout << "Graph: " << endl;

  for (unsigned int i=0 ; i<links.size() ; i++) {
    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      int dest   = links[i][j].first;
      double weight = links[i][j].second;
      if (type==WEIGHTED)
	cout << i << " " << dest << " " << weight << endl;
      else
	cout << i << " " << dest << endl;
    }
  }
}

void
Graph::display_binary(char *filename, int type) {
  ofstream foutput;
  foutput.open(filename,fstream::out | fstream::binary);

  int s = links.size();

  foutput.write((char *)(&s),4);

  int tot=0;
  for (unsigned int i=0 ; i<links.size() ; i++) {
    tot+=links[i].size();
    foutput.write((char *)(&tot),4);
  }

  for (unsigned int i=0 ; i<links.size() ; i++)
    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      int dest = links[i][j].first;
      foutput.write((char *)(&dest),4);
    }
  if (type==WEIGHTED) {
    for (unsigned int i=0 ; i<links.size() ; i++) {
      for (unsigned int j=0 ; j<links[i].size() ; j++) {
	double weight = links[i][j].second;
	foutput.write((char *)(&weight),sizeof(weight));
        /*if (i<10){
	   if (j<10){
		cerr << "weight = " << weight << ", ";
	   }
	}*/
      }
    }
  }
}

