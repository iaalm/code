/****************************************************************************************************************/
/*	Pocket M3N 0.11
/*	Copyright(c)2008 Media Computing and Web Intelligence LAB Fudan Univ.
/****************************************************************************************************************/
#ifndef M3N_H
#define M3N_H

#include <map>
#include <vector>
#include <string>
#include <math.h>
#include "freelist.h"
#include <string.h>

using namespace std;
const int LINEAR_KERNEL=0;
const int POLY_KERNEL=1;
const int RBF_KERNEL=2;
const int NEURAL_KERNEL=3;
const int LEARN_MODEL=0;
const int TEST_MODEL=1;

class str_length{
	public:
	size_t operator()(const char *str) const  {return strlen(str)+1;}
};
typedef freelist<char, str_length> charlist;



typedef struct templet{
	vector<string> words;
	vector<pair<int,int> > x;
	vector<int> y;
	bool end_of_group;
	int groupid;
}templet;
/*
define the templet "abc%x[1,0]xyz%x[-1,0]y[0]" in templet file
here,
words={"abc","xyz",""}
x={[1,0],[-1,0]}
y={0}

templets share the same y are clustered into one group:

%x[0,0]%y[0]			groupid=0	end_of_group=true
%x[0,0]%y[-1]%y[0]		groupid=1	end_of_group=false
x[-2,0]%y[-1]%y[0]		groupid=1	end_of_group=true
*/



class str_cmp{
	public:
	bool operator()(const char* s, const char* t) const 
	{
		return strcmp(s,t)<0;
	}
};


struct node;

typedef struct clique{
	int *fvector;
	int feature_num;
	node **nodes;
	int node_num;
	int groupid;
	int key;
}clique;
/*
for a concrete x, M3N use all templets for matching, and creates one clique for each templet group
fvector[i] ~ fvector[i] + ysize^node_num is the lambda space for the i th templet in the group
(ysize is the tag number, for "BIO" tagging, ysize=3)
feature_num is the group size.
nodes is the tokens linked by the clique, e.g. for "y[-1]y[0]" clique, the previous and current tokens are linked
node_num is the number of nodes, e.g. it is 2 for "y[-1]y[0]" clique
key: if the nodes are tagged, then clique.key is the index of the corresponding lambda in the clique's lambda space
e.g. for "y[-1]y[0]" clique, nodes are tagged as "IB", then
key = 2*3^1+0*3^0 = 6, i.e. r*f_i = lambda[fvector[i]+6] (f_i=1 here)
*/

typedef struct node{
public:
	clique **cliques;
	int clique_num;
	int key;
}node;

/*
cliques are the cliques linked with current node
key is the key path of current node
*/

typedef struct sequence{
	node* nodes;
	int node_num;
	int mu_index;
}sequence;

/*
node is the structure of token, path is the edge.
For "BIO" tagging, a sequence "x1...xn" contains n nodes,
if the order of CRF is 1, i.e. uses bigram features at most, then
the path number is n * 3 * 3

B	  .			.


I	  .			.


O	  .			.

	node_i	node_{i+1}

  corresponding paths: (i_B,{i+1}_B),(i_B,{i+1}_I),...(i_O,{i+1}_O)
*/


class M3N{
public:
	M3N();
	~M3N();
	bool set_para(char *, char *);
	bool learn(char* templet_file, char *training_file,char *model_file, bool relearn = false);
	bool load_model(char *model_file);

	void tag(vector<vector<string> > &table, vector<vector<string> > &best_tag,vector<double> &sequencep, vector<vector<double> > &nodep);
	void tag(vector<vector<string> > &table, vector<vector<string> > &best_tag);

private:
	double _C;	//slack variable penalty factor
	double _freq_thresh;
	double _eta;
	double *_mu;
	int _kernel_type;
	int _model;//0 for train, 1 for test
	int _version;
	int _mu_size;
	int _nbest;
	int _margin;
	vector<templet> _templets;
	vector<vector<vector<int> > > _templet_group;
	vector<vector<int> >_path2cliy;
	int _gsize;
	int _order;
	int _cols;
	int _ysize;
	int _path_num;
	int _node_anum;
	int _max_iter;

	int _w_size;	//useful iff linear kernel
	double *_w;		//useful iff linear kernel

	double _kernel_s;	// poly_kernel = (_kernel_s*linear_kernel+_kernel_r)^_kernel_d
	int _kernel_d;	// neural_kernel=tanh(_kernel_s*linear_kernel+_kernel_r)
	double _kernel_r;	// rbf_kernel = exp(-_kernel_s*norm2)


	map<char *, int, str_cmp> _xindex;//<"1:Confidence", 132> > 132 th x
	vector<int> _x_freq;//x_freq[i] is the frequency of the i th x
	charlist _x_str;//space storing x


	vector<char *> _tags;//sorted in alphabet order
	charlist _tag_str;//space storing tags
	
	vector<sequence> _sequences;
	freelist<node> _nodes;//storing all nodes
	freelist<clique> _cliques;//storing all cliques
	freelist<node*> _clique_node;//storing clique linked nodes' addresses
	freelist<clique*> _node_clique;//storing nodes linked cliques' addresses
	freelist<int> _clique_feature;//clique->feature

	freelist<node> _test_nodes;//storing all nodes
	freelist<clique> _test_cliques;//storing all cliques
	freelist<node*> _test_clique_node;//storing clique linked nodes' addresses
	freelist<clique*> _test_node_clique;//storing nodes linked cliques' addresses
	freelist<int> _test_clique_feature;//clique->feature

	int _print_level;//0, print base information; 1, print kkt_violation and obj after optimizing each sequence
	
	
	bool load_templet(char *templet_file);
	bool add_templet(char *line ,int &cur_group);
	bool check_training(char *training_file);
	void set_group();
	bool generate_features(char *training_file);
	bool add_x(vector<char *> &table);
	bool insert_x(char *target, int &index);
	void shrink_feature();
	void initialize();
	bool find_violation(sequence &seq, double &kkt_violation);
	void build_alpha_lattice(sequence &seq);
	void build_v_lattice(sequence &seq);
	bool write_model(char *model_file,bool first_part);
	void sort_feature(sequence &seq);
	void smo_optimize(sequence &seq);
	void generate_sequence(vector<vector<string> > &table, sequence &seq);
	void node_margin(sequence &seq, vector<vector<double> >&node_p,vector<double> &alpha, vector<double> &beta, double &z);
	double path_cost(sequence &seq, vector<double>& lattice);
	void assign_tag(sequence &seq, vector<int> &node_tag);
	//working temp variables
	vector<double> _v_lattice;
	vector<double> _alpha_lattice;
	vector<int> _path1,_path2;
	vector<double> _optimum_alpha_lattice;
	vector<vector<int> > _optimum_alpha_paths;
	vector<double> _optimum_v_lattice;
	vector<vector<int> > _optimum_v_paths;
	double _obj;
	vector<double> _clique_kernel;
	vector<double> _path_kernel;
	double _head_offset;


	//inline function
	
	void (M3N::*_get_kernel)(node &n1,node &n2);
	void (M3N::*_get_kernel_list)(node &n1,node &n2,int *path_list_1,int *path_list_2,int path_num);
	inline void get_kernel(node &n1,node &n2){
		return (this->*_get_kernel)(n1,n2);
	}
	inline void get_kernel(node &n1,node &n2,int *path_list_1,int *path_list_2,int path_num){
		return (this->*_get_kernel_list)(n1,n2,path_list_1,path_list_2,path_num);
	}
	//bool test(sequence &seq);
	inline int dot_product(int *f1, int fn1, int *f2, int fn2){
		register int sum = 0;
		int i,j;
		for(i=j=0;i<fn1 && j<fn2;){
			if(f1[i]==f2[j]){
				sum++;
				i++;j++;
			}else if(f1[i]<f2[j]){
				i++;
			}else{
				j++;
			}
		}
		return sum;
	}
	inline int dot_norm(int *f1, int fn1, int *f2, int fn2){
		register int sum = 0;
		int i,j;
		for(i=j=0;i<fn1 && j<fn2;){
			if(f1[i]==f2[j]){
				i++;j++;
			}else if(f1[i]<f2[j]){
				sum++;i++;
			}else{
				sum++;j++;
			}
		}
		sum+=fn1-i+fn2-j;
		return sum;
	}
	inline void get_clique_kernel(node &n1, node &n2){
		fill(_clique_kernel.begin(),_clique_kernel.end(),0);
		int i=0;
		int j=0;
		while(i<n1.clique_num && j<n2.clique_num){
			if(n1.cliques[i]->groupid==n2.cliques[j]->groupid){
				_clique_kernel[n1.cliques[i]->groupid]=dot_product(n1.cliques[i]->fvector,n1.cliques[i]->feature_num,n2.cliques[j]->fvector,n2.cliques[j]->feature_num);
				i++;j++;
			}else if(n1.cliques[i]->groupid<n2.cliques[j]->groupid){
				i++;
			}else{
				j++;
			}
		}
	}
	inline void get_clique_norm(node &n1, node &n2){
		fill(_clique_kernel.begin(),_clique_kernel.end(),0);
		int i=0;
		int j=0;
		while(i<n1.clique_num && j<n2.clique_num){
			if(n1.cliques[i]->groupid==n2.cliques[j]->groupid){
				_clique_kernel[n1.cliques[i]->groupid]=dot_norm(n1.cliques[i]->fvector,n1.cliques[i]->feature_num,n2.cliques[j]->fvector,n2.cliques[j]->feature_num);
				i++;j++;
			}else if(n1.cliques[i]->groupid<n2.cliques[j]->groupid){
				_clique_kernel[n1.cliques[i]->groupid]=n1.cliques[i]->feature_num;
				i++;
			}else{
				_clique_kernel[n2.cliques[j]->groupid]=n2.cliques[j]->feature_num;
				j++;
			}
		}
		while(i<n1.clique_num){
			_clique_kernel[n1.cliques[i]->groupid]=n1.cliques[i]->feature_num;
			i++;
		}
		while(j<n2.clique_num){
			_clique_kernel[n2.cliques[j]->groupid]=n2.cliques[j]->feature_num;
			j++;
		}
	}


	inline void linear_kernel(node &n1, node &n2){//get linear kernel matrix for a lattice unit
		int i,j,k;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<_path_num;i++){
			for(j=i%_ysize;j<_path_num;j+=_ysize){
				for(k=0;k<_gsize;k++){
					if(_clique_kernel[k]!=0 && _path2cliy[k][i]==_path2cliy[k][j]){
						_path_kernel[i*_path_num+j]+=_clique_kernel[k];
					}
				}
			}
		}
	}

	inline void linear_kernel(node &n1, node &n2, int *path_list_1, int *path_list_2, int path_num){//get linear kernel matrix for a lattice unit
		int i,j;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<path_num;i++){
			for(j=0;j<_gsize;j++){
				if(_clique_kernel[j]!=0 && _path2cliy[j][path_list_1[i]]==_path2cliy[j][path_list_2[i]]){
					_path_kernel[i]+=_clique_kernel[j];
				}
			}
		}
	}

	inline void poly_kernel(node &n1, node &n2){
		int i,j,k;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<_path_num;i++){
			for(j=i%_ysize;j<_path_num;j+=_ysize){
				for(k=0;k<_gsize;k++){
					if(_clique_kernel[k]!=0 && _path2cliy[k][i]==_path2cliy[k][j]){
						_path_kernel[i*_path_num+j]+=_clique_kernel[k];
					}
				}
				_path_kernel[i*_path_num+j]=pow(_kernel_s*_path_kernel[i*_path_num+j]+_kernel_r,_kernel_d);
			}
		}
	}
	inline void poly_kernel(node &n1, node &n2, int *path_list_1, int *path_list_2, int path_num){//get linear kernel matrix for a lattice unit
		int i,j;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<path_num;i++){
			for(j=0;j<_gsize;j++){
				if(_clique_kernel[j]!=0 && _path2cliy[j][path_list_1[i]]==_path2cliy[j][path_list_2[i]]){
					_path_kernel[i]+=_clique_kernel[j];
				}
			}
			_path_kernel[i]=pow(_kernel_s*_path_kernel[i]+_kernel_r,_kernel_d);
		}
	}
	inline void neural_kernel(node &n1, node &n2){
		//neural_kernel=tanh(_kernel_s+linear_kernel+_kernel_r)
		int i,j,k;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),tanh(_kernel_r));
		for(i=0;i<_path_num;i++){
			for(j=i%_ysize;j<_path_num;j+=_ysize){
				_path_kernel[i*_path_num+j]=0;
				for(k=0;k<_gsize;k++){
					if(_clique_kernel[k]!=0 && _path2cliy[k][i]==_path2cliy[k][j]){
						_path_kernel[i*_path_num+j]+=_clique_kernel[k];
					}
				}
				_path_kernel[i*_path_num+j]=tanh(_kernel_s*_path_kernel[i*_path_num+j]+_kernel_r);
			}
		}
	}
	inline void neural_kernel(node &n1, node &n2, int *path_list_1, int *path_list_2, int path_num){//get linear kernel matrix for a lattice unit
		int i,j;
		get_clique_kernel(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<path_num;i++){
			for(j=0;j<_gsize;j++){
				if(_clique_kernel[j]!=0 && _path2cliy[j][path_list_1[i]]==_path2cliy[j][path_list_2[i]]){
					_path_kernel[i]+=_clique_kernel[j];
				}
			}
			_path_kernel[i]=tanh(_kernel_s*_path_kernel[i]+_kernel_r);
		}
	}

	inline void rbf_kernel(node &n1, node &n2){
	//rbf_kernel = exp(-_kernel_s*norm2)
		int i,j,k;
		get_clique_norm(n1, n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<_path_num;i++){
			for(j=0;j<_path_num;j++){
				for(k=0;k<_gsize;k++){
					if(_path2cliy[k][i]==_path2cliy[k][j]){
						_path_kernel[i*_path_num+j]+=_clique_kernel[k];
					}else{
						_path_kernel[i*_path_num+j]+=n1.cliques[k]->feature_num+n2.cliques[k]->feature_num;
					}
				}
				_path_kernel[i*_path_num+j]=exp(-_kernel_s*_path_kernel[i*_path_num+j]);
			}
		}
	}
	inline void rbf_kernel(node &n1, node &n2, int *path_list_1, int *path_list_2, int path_num){//get linear kernel matrix for a lattice unit
		int i,j;
		get_clique_norm(n1,n2);
		fill(_path_kernel.begin(),_path_kernel.end(),0);
		for(i=0;i<path_num;i++){
			for(j=0;j<_gsize;j++){
				if(_path2cliy[j][path_list_1[i]]==_path2cliy[j][path_list_2[i]]){
					_path_kernel[i]+=_clique_kernel[j];
				}else{
					_path_kernel[i]+=n1.cliques[j]->feature_num+n2.cliques[j]->feature_num;
				}
			}
			_path_kernel[i]=exp(-_kernel_s*_path_kernel[i]);
		}
	}




};
#endif
