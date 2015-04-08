#pragma once
#include "engine/engineIncludes.h"
#include "engine/renderSys/Renderable.h"

namespace cm
{

		class Node;

		//////////////////////////////////////////////////////////////////////////
		/// A controller is a modifier to a node
		/// It can control it's transformation.
		class Controller : public Base
		{
		public:

			Controller()
			{
				active = true;
			}

			~Controller()
			{
				release();
			}

			virtual void release()
			{
			}


			virtual void	preTransform( Node * node, float msecs )
			{
			}

			virtual void	postTransform( Node * node, float msecs )
			{
			}

			bool	active;

		};

		
		//////////////////////////////////////////////////////////////////////////

		class Node : public Base
		{
		public:
			RTTIOBJECT( "Node", Node, Base )
			
			Node()
			:
			bRendering(false),
			visible(true)
			{
				parent=0;
				nodeTm.identity();
				worldTm.identity();
				controllers.clear();
				children.clear();
				
			}

			~Node()
			{
				release();
			}

			virtual void release()
			{
				controllers.clear();
				children.clear();
				parent=0;
			}

			/// local node matrix
			const cm::M44 & getNodeMatrix() const { return nodeTm; }
			/// concatenated world matrix
			const cm::M44 & getWorldMatrix() const { return worldTm; }

			virtual void	setNodeMatrix( const cm::M44 & m ) { nodeTm = m; }

			const cm::Vec3 & getPosition() const { return worldTm.trans(); }
			
			// quick transform funcs
			void identity() 
			{
				nodeTm.identity();
			}
			
			void rotate( float x, float y, float z )
			{
				nodeTm.rotate(radians(x),radians(y),radians(z));
			}
			
			void rotateX( float deg )
			{
				nodeTm.rotateX(radians(deg));
			}
			
			void rotateY( float deg )
			{
				nodeTm.rotateY(radians(deg));
			}
			
			void rotateZ( float deg )
			{
				nodeTm.rotateZ(radians(deg));
			}
			
			void translate( float x, float y, float z )
			{
				nodeTm.translate(x,y,z);
			}
			
			void scale( float x, float y, float z )
			{
				nodeTm.scale(x,y,z);
			}
			
			void scale( float s )
			{
				nodeTm.scale(s,s,s);
			}


			void flatten( std::vector<Node*> &flat, Node * n )
			{
				flat.push_back(n);
				for( int i = 0; i < n->getNumChildren(); i++ )
					flatten(flat,n->getChild(i));
			}
			

			Node* beginTraverse()
			{
				current = 0;
				//traverse(viewMatrix,msecs);
				flatNodes.clear();
				flatten(flatNodes,this);
				return traverse();
			}

			Node* traverse()
			{	
				if( current < flatNodes.size() )
					return flatNodes[current++];

				return 0;
			}

			void traverse( const cm::M44 & viewMatrix ,double msecs )
			{
				bRendering = false;
				traverse(viewMatrix,msecs,0,false);
			}
			
			void transformAndRender( const cm::M44 & viewMatrix , double msecs )
			{
				bRendering = true;
				traverse(viewMatrix,msecs,0,true);
			}
			///
			
			virtual void setTransform( const cm::M44 & parentTm = cm::M44::identityMatrix() )
			{
				worldTm = parentTm*nodeTm;
				if(bRendering)
					applyMatrix(nodeTm);
			}
			
			virtual void render( int depth  )
			{
			}
			
			virtual void update() {}
			
			int		getNumControllers() const { return controllers.size(); }
			void			addController( Controller * c ) { controllers.push_back(c); }
			Controller * getController( int i ) { return controllers[i]; }

			int		getNumChildren() const { return children.size(); }
			void			addChild( Node * n )
			{
				n->parent = this;
				children.push_back(n);
			}

			bool removeChild( Node * n )
			{
				std::vector< RefPtr<Node> >::iterator it;

				for( it = children.begin(); it != children.end(); it++ )
				{
					if( it->ptr == n )
					{
						children.erase(it);
						return true;
					}
				} 

				return false;
			}
			
			Node		 * getChild( int i ) { return children[i]; }

			Node		 * getParent() { return parent; }

			void			setDirty( bool dirty = true ) { dirty = dirty; }
			bool			isDirty() const { return dirty; }

			bool isVisible() const { return visible; }
			void setVisible( bool flag )
			{
				visible = flag;
				for( int i = 0; i < getNumChildren(); i++ )
					getChild(i)->setVisible(flag);
			}
			
			
		protected:
			// need push pop matrix to make this recursive!!!!
			
			/// traverse scene graph
			void	traverse( const cm::M44 & mat,  double msecs, int depth, bool bRender = true )
			{
				//for( int i = 0; i < depth; i++ )
				//	printf(" ");
				//printf("Traversing %d children:%d\n",depth,getNumChildren());
				
				if( bRender )
					pushMatrix();
				
				for(int i = 0; i < getNumControllers(); i++)
				{
					Controller*c=controllers[i];
					c->preTransform(this,msecs);
				}
				
				//applyMatrix(nodeTm);
				setTransform(mat);
				
				update();
				
				if(bRender)
				{
//					setTransform();
					render(depth);
				}
				
				if( depth < maxDepth )
				{
					for(int i = 0; i < getNumChildren(); i++)
					{
						children[i]->traverse(worldTm,msecs,depth+1,bRender);
					}
				}
				
				for(int i = 0; i < getNumControllers(); i++)
				{
					Controller*c=controllers[i];
					c->postTransform(this,msecs);
				}
				
				if( bRender )
					popMatrix();
			}


			static int maxDepth;
		protected:
			bool bRendering;
			bool visible;
			
			// Iteration utils
			int current;
			std::vector<Node*> flatNodes;

			Node *							parent;
			std::vector< RefPtr<Node> >		children;
			std::vector< RefPtr<Controller> > controllers;

			cm::M44 nodeTm;
			cm::M44 worldTm;

			bool dirty;
		};
		
		
		class RenderableNode : public Node
		{
		public:
			RTTIOBJECT( "RenderableNode", RenderableNode, Node )
			
			RenderableNode()
			{
				objects.release();
			}

			~RenderableNode()
			{
				release();
			}

			virtual void release()
			{
				objects.release();
				Node::release();
			}
			
			virtual void render( int depth )
			{
				if(!visible)
					return;
					
				for( int i = 0; i < getNumObjects(); i++ )
				{
					objects[i]->render();
				}
			}
			
			int	 		getNumObjects() const { return objects.size(); }
			void 		addRenderable( Renderable * o ) { objects.add(o); }
			const Renderable * getRenderable( int i) const { return objects[i]; }
			Renderable * getRenderable( int i) { return objects[i]; }
				
		public:
			Group<Renderable> objects;
		};



}