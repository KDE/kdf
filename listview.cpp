/*
 *   Copyright (C) 1999  Espen Sand, espen@kde.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

//
// 1999-11-28 Espen Sand 
// The purpose of this class is:
// 1) Easily set minimum number of visible items and to adjust the sizeHint()
// 2) Provide a pixmap collection
//

#include <qbitmap.h>
#include <qheader.h>

#include <kapp.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include "listview.h"

template QDict<QPixmap>;

CListView::CListView( QWidget *parent, const char *name, int visibleItem )
  :QListView( parent, name ),  mVisibleItem(QMAX( 1, visibleItem ))
{
  setVisibleItem(visibleItem);
  mPixDict.setAutoDelete(true);
}

void CListView::setVisibleItem( int visibleItem, bool updateSize )
{
  mVisibleItem = QMAX( 1, visibleItem );
  if( updateSize == true )
  {
    QSize s = sizeHint();
    setMinimumSize( s.width() + verticalScrollBar()->sizeHint().width() + 
		    lineWidth() * 2, s.height() );
  }
}

QSize CListView::sizeHint( void ) const
{
  QSize s = QListView::sizeHint();
  
  int h = fontMetrics().height() + 2*itemMargin();
  if( h % 2 > 0 ) { h++; }
  
  s.setHeight( h*mVisibleItem + lineWidth()*2 + header()->sizeHint().height());
  return( s );
}



const QPixmap &CListView::icon( const QString &iconName, bool drawBorder )
{
  QPixmap *pix = mPixDict[ iconName ];
  if( pix == 0 )
  {
    KIconLoader &loader = *KGlobal::iconLoader();
    
    pix = new QPixmap( 
      loader.loadApplicationIcon( iconName, KIconLoader::Small, 0, false ) );
    if( drawBorder == true )
    {
      QBitmap *bm = new QBitmap(*(pix->mask()));
      if( bm != 0 ) 
      {
	QPainter qp(bm);
	qp.setPen(QPen(white,1));
	qp.drawRect(0,0,bm->width(),bm->height());
	qp.end();
	pix->setMask(*bm);
      }
      QPainter qp(pix); 
      qp.setPen(QPen(red,1));
      qp.drawRect(0,0,pix->width(),pix->height());
      qp.end();
    }
    mPixDict.replace( iconName, pix );
  }

  return( *pix );
}


#include "listview.moc"




