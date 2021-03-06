/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-classical
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCTMXObjectDebugRenderer.h"
#include "CCTMXTiledMap.h"
#include "CCTMXObjectGroup.h"
#include "CCTMXObject.h"
#include "cocoa/CCPointExtension.h"
#include "draw_nodes/CCDrawingPrimitives.h"

NS_CC_BEGIN

CCTMXObjectDebugRenderer::CCTMXObjectDebugRenderer() :
m_map(NULL) {
}

CCTMXObjectDebugRenderer::~CCTMXObjectDebugRenderer() {
}

CCTMXObjectDebugRenderer* CCTMXObjectDebugRenderer::create(CCTMXTiledMap* m) {
	CCTMXObjectDebugRenderer* l = new CCTMXObjectDebugRenderer();
	if(l->initWithMap(m)) {
		CC_SAFE_AUTORELEASE_RETURN(l, CCTMXObjectDebugRenderer*);
	}
	CC_SAFE_RELEASE(l);
	return NULL;
}

bool CCTMXObjectDebugRenderer::initWithMap(CCTMXTiledMap* map) {
	m_map = map;
	return true;
}

void CCTMXObjectDebugRenderer::draw() {
	CCTMXMapInfo* mapInfo = m_map->getMapInfo();
	CCObject* obj;
	CCARRAY_FOREACH(&mapInfo->getObjectGroups(), obj) {
		CCTMXObjectGroup* group = (CCTMXObjectGroup*)obj;
		int oc = group->getObjectCount();
		for(int i = 0; i < oc; i++) {
			CCTMXObject* obj = group->getObjectAt(i);
			
			// get object x, y, those x, y is in tmx coordinates
			CCPoint loc = obj->getPosition();
			
			// is object a polygon?
			bool isNormal = obj->getShape() == CCTMXObject::NORMAL;
			bool isClosed = obj->getShape() == CCTMXObject::POLYGON;
			
			// fill vertex buffer
			m_vertices.clear();
			if(isNormal) {
				// manual add point, remember currently it should be in tmx coordinate space
				CCSize s = obj->getSize();
				m_vertices.addPoint(0, 0);
				m_vertices.addPoint(0, s.height);
				m_vertices.addPoint(s.width, s.height);
				m_vertices.addPoint(s.width, 0);
				m_vertices.addPoint(0, 0);
			} else {
				// add points
				m_vertices.addPoints(obj->getPoints());
				
				// if closed, add first point
				if(isClosed) {
					m_vertices.addPoint(obj->getPoints().getPointAt(0));
				}
			}
			
			// convert all points from tmx space to node space
			CCPoint* buffer = m_vertices.getBuffer();
			for(int i = 0; i < m_vertices.getCount(); i++) {
				CCPoint p = m_map->tmxToNodeSpace(ccp(buffer[i].x + loc.x, buffer[i].y + loc.y));
				buffer[i].x = p.x;
				buffer[i].y = p.y;
			}
			
			// draw lines
			ccDrawColor4B(0, 255, 0, 255);
			ccDrawPoly(buffer, m_vertices.getCount(), false);
		}
	}
}

NS_CC_END