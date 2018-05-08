




/**

#ifndef QuadTree_HPP
#define QuadTree_HPP

enum QUAD_NODE {
   NODE_NW = 0,
   NODE_NE = 1,
   NODE_SW = 2,
   NODE_SE = 3,
   NUM_NODES = 4
};

class QTNode {
   double x,y,w,h;
   bool empty;
   
   QTNode* quads[NUM_NODES];
   
///   QTNode(double xpos , double ypos , double width , double height);
   QTNode(double xpos , double ypos , double width , double height) :
         x(xpos),
         y(ypos),
         w(width),
         h(height),
         empty(true),
         quads()
   {
      memset(quads , 0 , sizeof(QTNode*)*NUM_NODES);
   }
   
   bool Hit(double xpos , double ypos);
};

class QuadTree {
   QTNode root;
   
   QuadTree(double x , double y , double width , double height);
   QuadTree(double x , double y , double width , double height) :
         root(x,y,width,height)
   {}
   
   QTNode* GetNode(
   
   bool Hit(int x , int y);
bool Hit(int x , int y) {
   return root.Hit(x + 0.5 , y + 0.5);
}
   
};


#endif // QuadTree_HPP

//*/
