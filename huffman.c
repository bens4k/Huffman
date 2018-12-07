#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct forest Forest;
typedef struct tree Tree;
typedef struct node Node;

typedef struct forest {
    Tree *first, *end;
} Forest;

typedef struct tree{
	Node* head;
	Tree* next;
} Tree;

typedef struct node {
    char* key;
    float prob;
    char  code;
    Node* parent;
    Tree* children[2];
} Node;

// node functions
Node* new_node(char* k, float p);
void free_node(Node* n);
void print_node(Node* n);
Node* clone_node(Node* n);

// forest functions
Forest* new_forest();
void print_forest(Forest* f);
void add_tree_to_forest(Forest* f, Tree* t);
void remove_tree_from_forest(Forest* f, Tree* t);
void move_down(Forest* f, Tree* t);
void sort_forest(Forest* f);
unsigned int size_of_forest(Forest* f);

// tree functions
Tree* new_tree(Node* n);
Tree* combine_trees(Tree* t1, Tree* t2);
void print_tree(Tree* t);
Tree* clone_tree(Tree* t);

//huffman
void merge_lowest_and_update_codes(Forest* f);
void huffman(Forest* f);
char* get_code_for(Forest* f, char* k);

int main(){
	unsigned int count = 5;
	char* chars[] = {"a1", "a2", "a3", "a4", "a5"};
	float probs[] = {0.15, 0.04, 0.26, 0.05, 0.5};
	
	Forest* fst = new_forest();
	for(int i=0 ; i<count ; i++){
		add_tree_to_forest( fst, new_tree( new_node(chars[i], probs[i]) ));
	}
    huffman(fst);
	float avg_length = 0;
	float entropy = 0;
    for(int i=0; i<count; i++){
		entropy += probs[i] * log(probs[i])/log(2);
		char* code = get_code_for(fst, chars[i]);
		avg_length += strlen(code) * probs[i];
		printf("code for %s is: %s\n", chars[i], code);
	}
	entropy *= -1 ;
	printf("Entropy = %f \n", entropy);
	printf("Average length = %f \n", avg_length);
    return 0;
}
char contains(char* str, char* s){
	return (strstr(str, s) != NULL) ? 1 : 0;
}
char digit_to_char(const char d){
	if((d <= 9) && (d >= 0)){
		return d + 48;
	}
	return 0;
}
char* reverse_string(char* str){
	unsigned int len = strlen(str);
	char* new_str = malloc(len);
	for(int i=0; i<len; i++){
		new_str[len-i-1] = str[i];
	}
	return new_str;
}
char* get_code_for(Forest* f, char* k){
	char code[100] = {0};
	unsigned char index = 0;
	if(f != NULL){
		Tree* t = f->first;
		while(1){
			if(t->head->code != -1)
				code[index++] = digit_to_char(t->head->code);
			if(strcmp (t->head->key, k) == 0){
				return reverse_string(code);
			}else{
				Tree* left = t->head->children[0];
				Tree* right = t->head->children[1];
				if (contains(left->head->key, k))
					t = left;
				else if (contains(right->head->key, k))
					t = right;
				else break;
			}
		}
	}
	return NULL;
}
unsigned int size_of_forest(Forest* f){
    unsigned int s = 0;
    if(f != NULL){
        Tree* t = f->first;
        while(t != NULL){
            s += 1;
            t = t->next;
        }
    }
    return s;
}
void merge_lowest_and_update_codes(Forest* f){
    if(f != NULL){
        Tree* t = f->first;
        while (t->next->next != NULL)
            t = t->next;
        // merge
        Tree* new_t = combine_trees(t, t->next);
        // update codes
        if(t->head->prob > t->next->head->prob) {
            new_t->head->children[0]->head->code = 1;
            new_t->head->children[1]->head->code = 0;
        }else{
            new_t->head->children[0]->head->code = 0;
            new_t->head->children[1]->head->code = 1;
        }
        // remove trees
        remove_tree_from_forest(f, t->next);
        remove_tree_from_forest(f, t);
		add_tree_to_forest(f, new_t);
    }
}
void huffman(Forest* f){
    while(size_of_forest(f) > 1){
        sort_forest(f);
        merge_lowest_and_update_codes(f);
    }
}

void print_node(Node *n){
    printf("('%s', %f, %d) ", n->key, n->prob, n->code);
}
void print_tree(Tree* t){
    if(t != NULL){
        printf("{");
        if(t->head->children[0] != NULL){
            print_tree(t->head->children[0]);
            printf("<-");
        }
        if(t->head != NULL)
            print_node(t->head);
        if(t->head->children[1] != NULL){
            printf("->");
            print_tree(t->head->children[1]);
        }
        printf("}");
    }
}
void print_forest(Forest* f){
    if(f != NULL){
        Tree* t = f->first;
        while(t != NULL){
            print_tree(t);
            t = t->next;
        }
        printf("\n");
    }else printf("forest empty!\n");
}
Forest* new_forest(){
    Forest* f = malloc(sizeof(Forest));
    f->first = NULL;
    f->end = NULL;
    return f;
}
void add_tree_to_forest(Forest* f, Tree* t){
    if(f != NULL){
        if(f->first == NULL){
            f->first = t;
            f->end = t;
        }else{
            f->end->next = t;
            f->end = t;
        }
    }
}
Tree* new_tree(Node* n){
    Tree* t = malloc(sizeof(Tree));
    t->head = n;
    t->next = NULL;
    return t;
}
Node* clone_node(Node* n){
    char* new_key = malloc( strlen(n->key) + 1);
    sprintf(new_key, "%s", n->key);
    Node* new_n = new_node(new_key, n->prob);
	new_n->code = n->code;
    //new_n->parent = n->parent;
    if(n->children[0] != NULL)
        new_n->children[0] = clone_tree(n->children[0]);
    if(n->children[1] != NULL)
        new_n->children[1] = clone_tree(n->children[1]);
    return new_n;
}
Tree* clone_tree(Tree* t){
    Node* new_h = clone_node(t->head);
    return new_tree(new_h);
}
Node* new_node(char* k, float p){
    Node* n = malloc(sizeof(Node));
    n->key = k;
    n->prob = p;
    n->code = -1;
    n->parent = NULL;
    n->children[0] = NULL;
    n->children[1] = NULL;
    return n;
}
void free_node(Node* n){
    if(n != NULL)
        free(n);
}
void free_tree(Tree* t){
    if(t->head->children[0] != NULL)
        free_tree(t->head->children[0]);
    if(t->head->children[1] != NULL)
        free_tree(t->head->children[1]);
    free_node(t->head);
}
void remove_tree_from_forest(Forest* f, Tree* t){
    if((f != NULL) && (t != NULL)){
        // target is first
        if(f->first == t){
            if(t->next == NULL){// single tree Forest
                f->first = NULL;
                f->end = NULL;
            }else
                f->first = t->next;
            //free_tree(t);
            return;
        }else{
            // look for it
            Tree* mt = f->first;
            while (mt->next != NULL){
                if(mt->next == t){
                    mt->next = t->next;
                    if(t->next == NULL)
                        f->end = mt;
                    //free_tree(t);
                    return;
                }
                mt = mt->next;
            }
        }
    }
}
Tree* combine_trees(Tree* t1, Tree* t2){
    Tree* new_t = NULL;
    if((t1 != NULL) && (t2 != NULL)){
        unsigned int len = strlen(t1->head->key) + strlen(t2->head->key);
        char* mm = malloc(sizeof(len)+1);
        sprintf(mm, "%s%s",t1->head->key,t2->head->key);

        Node* new_n = new_node(mm, t1->head->prob + t2->head->prob);
        new_n->children[0] = clone_tree(t1);
        new_n->children[1] = clone_tree(t2);
        t1->head->parent = new_n;
        t2->head->parent = new_n;
        new_t = new_tree(new_n);
    }else
		printf("warning merging with empty trees! \n");
    return new_t;
}
void move_down(Forest* f, Tree* t){
    if( (f != NULL) && (f->first != NULL)){
        if((f->first == t) && (f->first->next != NULL)){
            Tree* a = t;
            Tree* b = t->next;
            f->first = b;
            a->next = b->next;
            b->next = a;
        }else{
            Tree* tt = f->first;
            while (tt->next != NULL){
                if (tt->next == t){
                    // in case T is not at the end
                    if(t->next != NULL){
                        Tree* tmp = t->next->next;
                        tt->next = t->next;
                        t->next->next = t;
                        t->next = tmp;
                    }
                    // in case T became last after move
                    if(t->next == NULL){
                        f->end = t;
                    }
                    break;
                }
                tt = tt->next;
            }
        }
    }
}
void sort_forest(Forest* f){
    if((f != NULL) && (f->first != NULL)){
        unsigned int swaps;
        while(1){
            swaps = 0;
            Tree* m = f->first;
            while(m != NULL){
                if(m->next != NULL){
                    if((m->head->prob) < (m->next->head->prob)){
                        move_down(f, m);
                        swaps += 1;
                    }
                }
                m = m->next;
            }
            if(swaps == 0)
                break;
        }
    }
}
