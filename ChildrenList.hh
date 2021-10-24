#include "RexpNode.hh"
class CNode;
class CList;
class RNode;

class CNode {
    public:
        RNode * self;
        CNode * next;
        
        
        CNode(RNode * rnodeIn) : self(rnodeIn), next(this) {}//maybe next(self)?

        CNode(CNode * cnodeIn) {
            self = new RNode(cnodeIn->self);
            next = nullptr;
        }

        ~CNode() {
            delete self;
        }

};

class CList {
    public:
        CNode * head;
        CNode * tail;


        CList(CList * c) {

            if(c == nullptr) {
                //empty list
                head = nullptr;
                tail = nullptr;
            }
            head = new CNode(c->head);
            CNode * ccopy = head;
            CNode * cnext= c->head->next;
            while(cnext != c->head) {
                CNode * copynext(cnext);
                ccopy->next = copynext;
                ccopy = copynext;
                cnext = cnext->next;
            }
            ccopy->next = head;
            tail = ccopy;
            //next = nullptr;
            //self = new RNode(c->self);
        }
        CList(RNode * r1, RNode * r2) {
            head = new CNode(r1);
            tail = new CNode(r2);
            head->next = tail;
            tail->next = head;
        }

        CList(RNode * r) {
            head = new CNode(r);
            tail = head;
            head->next = tail;
        }



        CList() = default;

        ~CList() {
            CNode * ptr, * ptr_prev;
            for(ptr = head; ptr->next != head; ) {
                ptr_prev = ptr;
                ptr = ptr->next;
                delete ptr_prev;
            }
            delete ptr;
        }
};

