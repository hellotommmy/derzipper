
#include "ChildrenList.hh"
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using std::endl;
using std::cout;
using std::vector;
using std::string;

#define MAXBITLISTLEN 64
#define BITSTORE (MAXBITLISTLEN / ((sizeof(uint)) * 8))
#define CLRB(node)  ({node->bits[0] = 0; for(int i = 0; i < BITSTORE; i ++){node->bits[i+1] = 0;}})


typedef enum tpe type;
typedef enum tkens tokens;
class CNode;
class CList;
class RNode;
RNode * deepcopy(RNode * r);
CList * deepcopy_children(CList * c);

RNode * RNode::getChild(int ith) {
    if(ith == 1)
        return this->children->head->self;
    else
        return this->children->tail->self;
}

void expect(char c, char * t, int * p) {
    if(t[*p] == c) {
        *p = *p + 1;
    }
    else {
        throw std::invalid_argument("expected" +  string(1, c) +  "surrounding COMP but got" + string(1, t[*p]) );
    }
}

RNode::~RNode() {
    if(type == SEQ || type == STAR || type == ALT)
        delete children;
    if(type != ZERO)//if(bits != nullptr)//when bits has been allocated with something
        delete [] bits;
}

RNode::RNode(RNode * e1, RNode * e2, enum tpe t) : type(t), data('#'), bits(nullptr), children(nullptr){
    CList * c1 = new CList(e1, e2);
    children = c1;
    bits = new uint[BITSTORE + 1];
    CLRB(this);
}
RNode * compParser(char * t, int * p, RNode * e, tpe tp) {
    *p = *p + 1; 
    RNode * e2 = parseRexpInput(t, p); 
    RNode * e3 = new RNode(e, e2, tp);
    expect(')', t, p); 
    return e3;
}

//star constructor
RNode::RNode(char * t, int * p, RNode * e) : type(STAR), children(nullptr), bits(nullptr), data('#') {
    type = STAR;
    children = new CList(e);
    bits = new uint[BITSTORE + 1];
    CLRB(this);
    *p = *p + 1;
}

RNode::RNode(char * t, int * p, char c) : type(CHAR), children(nullptr), bits(nullptr), data(t[*p]) {
    bits = new uint[BITSTORE + 1];
    CLRB(this);
    //assertion
    if(t[*p] != c)
        throw std::invalid_argument("expected" +  string(1, c) +  " but got" + string(1, t[*p]) );
    *p = *p + 1;
}

RNode * parseRexpInput(char * t, int * p) {
    int status = 0;
    switch(t[*p]) {
        case '(': {
            *p = *p + 1;
            RNode * e = parseRexpInput(t, p);
            //printf("a%d\n", *p);
            if(t[*p] == '+') {
                return compParser(t, p, e, ALT);
            } else if(t[*p] == ')') {
                // it is a star node--only 1 child
                return new RNode(t, p, e);
            } else if(t[*p] == '.') {
                return compParser(t, p, e, SEQ);
            }
            else {
                throw std::invalid_argument("expected (, +, or . but got" + string(1, t[*p]) );
            } 
            break;
        }
        default: {
            char c = t[*p];
            if('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
                return new RNode(t, p, c);
            }
            else {
                throw std::invalid_argument("seen " + string(1, c));
                return NULL;
            }
        }
    }
}

class Tree {
    public:
        RNode * root;
        RNode * focus;
};


void print_child(CList * head, int depth);
void print_expr(RNode * e, int depth);
// RNode * expr_S(char * t, int * p);





// RNode * expr_COMPSTR(char * t, int * p, RNode * e1) {
//     switch(t[*p]) {
//         case '+' :
//             *p = *p + 1;
//             RNode * e2 = expr_S(t, p);
//             RNode * e3 = create_comp_node(e1, e2, ALT);
//             return expect(')', t, p, e3);
//         case ')' :
//             *p = *p + 1;
//             RNode * e = new RNode();
//     }
// }


//each time expr_S finishes it should
//move p to some unread token that is
//one token ahead the most recently
//parsed token


void set_bits(RNode * child, bool bit) {
    CLRB(child);
    child->bits[0] = 1;
    if(bit) {
        child->bits[1] = 1;
    }
    else {
        child->bits[1] = 0;
    }
}

void internalise(RNode * expr) {
    if(expr->type != ZERO)
        CLRB(expr);
    switch(expr->type) {
        case ALT:{
            CNode * child1 = expr->children->head;
            CNode * child2 = child1->next;
            //CLRB(expr);
            set_bits(child1->self, false);
            set_bits(child2->self, true);
            break;
        }
        case SEQ: {
            //CLRB(expr);
            internalise(expr->children->head->self);
            internalise(expr->children->head->next->self);
            break;
        }
        case STAR: {
            //CLRB(expr);
            internalise(expr->children->head->self);
            break;
        }
        default: {
            break;
        }
            
    }
}


inline void print_indent(int depth) {
    for(int i = 0; i < depth; i++){
        printf("\t");
    }
}

inline void print_compound_node(const char * literal, RNode * e, int depth){
    printf("%s\n", literal);
    print_child(e->children, depth + 1);
}

void print_child(CList * cl, int depth){
    if(cl == nullptr) return;

    CNode * head = cl->head;
    if(head == nullptr) return;
    CNode * current_child;
    bool loopBodyExecuted = false;
    for(current_child = head;  current_child != head || !loopBodyExecuted; current_child = current_child->next) {
        if(current_child->self)
            print_expr(current_child->self, depth);
        loopBodyExecuted = true;
    }
}
void print_bits(RNode * e) {
    for(int i = 0; i < e->bits[0]; i++) {
        if(e->bits[i + 1] == 0) // ₀  ₁
            cout << "₀";
        else
            cout << "₁";
    }
}

void print_expr(RNode * e, int depth) {
    print_indent(depth);
    if(e->type != ZERO && e->bits[0] > 0)
        print_bits(e);
    switch(e->type) {
        case SEQ :
            print_compound_node("SEQ", e, depth);
            break;
        case ALT :
            print_compound_node("ALT", e, depth);
            break;
        case STAR:
            print_compound_node("STAR", e, depth);
            break;
        case CHAR:
            printf("%c\n", e->data);
            break;
        case ZERO:
            printf("∅\n");
            break;
        case ONE:
            printf("ϵ\n");
            break;
    }
}



// inline CList * circular_children(RNode * r1, RNode * r2) {
//     CList * c1 = new CList();
//     CList * c2 = new CList();
//     c1->next = c2;
//     c2->next = c1;
//     c1->self = r1;
//     c2->self = r2;
//     return c1;
// }

RNode * wrap(RNode * r1, RNode * r2, type t, uint * b) {
    RNode * r = new RNode();
    cout << r << endl;
    r->type = t;
    //robs the memory (the bits stored inside) being pointed by b
    //in the caller function need to give b a new memory to point to
    r->bits = b;
    // for(int i = 0; i < BITSTORE + 1; i++) {
    //     r->bits[i] = b[i];
    // }
    r->children = new CList(r1, r2);
    r->data = '#';
    return r;
}

bool nullable(RNode * r) {
    switch(r->type){
        case SEQ:
            return nullable(r->children->head->self) && nullable(r->children->head->next->self);
        case ALT:{
            // cout << "alternative expression nullable?" << endl;
            // print_expr(r, 0);
            // print_expr(r->children->head->self, 0);
            // print_expr(r->children->head->next->self, 0);
            // if(nullable(r)) cout << "r is nullable"; else cout << "r is not nullable";
            return nullable(r->children->head->self) || nullable(r->children->head->next->self);
        }
        case STAR:
            return true;
        case ONE:
            return true;
        case CHAR:
            return false;
        case ZERO:
            return false;
        default:
            return false;
    }
}


CList *  deepcopy_children(CList * c) {
            if(c == nullptr) {
                return nullptr;
                //empty list
                // newc->head = nullptr;
                // newc->tail = nullptr;
                
            }
            CList * newc = new CList();


            newc->head = new CNode(c->head);
            CNode * ccopy = newc->head;
            
            CNode * cnext= c->head->next;
            while(cnext != c->head) {
                CNode * copynext = new CNode(cnext);
                ccopy->next = copynext;
                ccopy = copynext;
                cnext = cnext->next;
            }
            ccopy->next = newc->head;
            newc->tail = ccopy;
            return newc;
}
// CList * deepcopy_children(CList * c) {
//     if(c == nullptr)
//         return nullptr;
//     CList * ccopy = new CList(c);

//     CList * cc = ccopy;

//     CList * cnext= c->;

//     while(cnext != c) {
//         CList * copynext(cnext);
//         ccopy->next = copynext;
//         ccopy = copynext;
//         cnext = cnext->next;
//     }
//     ccopy->next = cc;

//     return cc;
// }

// RNode * deepcopy(RNode * r) {
//     RNode * newr = new RNode();
//     switch(r->type) {
//         case SEQ: {
//             CList * c = deepcopy_children(r->children);
//             newr->children = c;
//             newr->bits = new uint[BITSTORE + 1];
//             CLRB(newr);
//             for(int i = 0; i < BITSTORE + 1; i++) {
//                 newr->bits[i] = r->bits[i];
//             }
//             newr->data = '#';
//             newr->type = SEQ;
            
//         }
//     }
// }


// scala version
//   def bder(c: Char, r: ARexp) : ARexp = r match {
//     case AZERO => AZERO
//     case AONE(_) => AZERO
//     case APRED(bs, f) => if (f(c)) AONE(bs:::List(C(c))) else AZERO
//     case AALTS(bs, rs) => AALTS(bs, rs.map(bder(c, _)))
//     case ASEQ(bs, r1, r2) => 
//       if (bnullable(r1)) AALT(bs, ASEQ(Nil, bder(c, r1), r2), fuse(mkepsBC(r1), bder(c, r2)))
//       else ASEQ(bs, bder(c, r1), r2)
//     case ASTAR(bs, r) => ASEQ(bs, fuse(List(S), bder(c, r)), ASTAR(Nil, r))
//   }
RNode * fuse(bool bit, RNode * r) {
if(r->type != ZERO){    
    if(bit)
        r->bits[r->bits[0] + 1] = 1;
    else
        r->bits[r->bits[0] + 1] = 0;
    r->bits[0]++;
}
return r;
}

RNode * simp(RNode * r) {
    switch(r->type) {
        case ONE: 
            return r;
        case ZERO:
            return r;
        case CHAR:
            return r;
        default: {
            return r;
        }
    }
}

RNode * der(char c, RNode * r) {
    switch(r->type) {
        case SEQ:{
            //cout << "hello from SEQ BRANCH of der";
            RNode * r1 = r->getChild(1);//r->children->head->self;
            RNode * r2 = r->getChild(2);//r->children->head->next->self;
            //cout << rexp_types[r1->type] << endl;
            //cout << rexp_types[r2->type] << endl;
            if(nullable(r1)){
                //cout << "this branch executed when doing dervative to" << endl;
                //print_expr(r, 0);
                
                RNode * r2d = der(c, new RNode(r2) );
                RNode * r1d = der(c, r1);
                r->children->head->self = r1d;
                //CList * newchildren = new CList(r, r2d);
                //move r1.r2's bitsequence up to r1\cr2 + r2\c node
                RNode * r1dr2_r2d = wrap(r, r2d, ALT, r->bits);
                r->bits = new uint[BITSTORE + 1];
                CLRB(r);
                return r1dr2_r2d;
            }
            else {
                RNode * r1d = der(c, r1);
                r->children->head->self = r1d;
                return r;
            }
        }
        case CHAR:{
            if(r->data == c)
                r->type = ONE;
            else{
                r->type = ZERO;
                delete [] r->bits;
                r->bits = nullptr;
            }
            return r;
        }
        case STAR: {
            RNode * r0 = new RNode(r->getChild());
            RNode * r0p = fuse(true, der(c, r0));
            RNode * r0p_starr0 = wrap(r0p, r, SEQ, r->bits);
            r->bits = new uint[BITSTORE + 1];
            CLRB(r);
            return r0p_starr0;

        }
        case ALT: {
            // RNode * r1 = r->children->head->self;
            // RNode * r2 = r->children->head->next->self;
            RNode * r1 = r->getChild(1);
            RNode * r2 = r->getChild(2);
            RNode * r1d = der(c, r1);
            RNode * r2d = der(c, r2);
            r->children->head->self = r1d;
            r->children->head->next->self = r2d;
            return r;
        }
        case ONE: {
            r->type = ZERO;
            delete [] r->bits;
            return r;
        }
        case ZERO: {
            return r;
        }
        default:
            return r;
    }
}

int main() {
    char t[] = "((a+(a.b)))*";
    //printf("%s", t);
    int pos = 0;
    RNode * e = parseRexpInput(t, &pos);
    //cout << e->type <<"\n";

    internalise(e);
    // print_expr(e, 0);

    // print_expr(e, 0);
    RNode * ed = der('a', e);
    //print_expr(ed, 0);
    RNode * edd = der('b', ed);
    print_expr(edd, 0);
    delete edd;

    return 0;
    
}
