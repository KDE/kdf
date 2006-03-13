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
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

//
// 1999-11-28 Espen Sand
// The purpose of this class is:
// 1) Easily set minimum number of visible items and to adjust the sizeHint()
// 2) Provide a pixmap collection
//

#include <qbitmap.h>
#include <q3header.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPixmap>

#include <kiconloader.h>

#include "listview.h"

template class Q3Dict<QPixmap>;

CListView::CListView( QWidget *parent, const char *name, int visibleItem )
  :K3ListView( parent ),  mVisibleItem(qMax( 1, visibleItem ))
{
  setObjectName(name);
  setVisibleItem(visibleItem);
  mPixDict.setAutoDelete(true);
}

void CListView::setVisibleItem( int visibleItem, bool updateSize )
{
  mVisibleItem = qMax( 1, visibleItem );
  if( updateSize == true )
  {
    QSize s = sizeHint();
    setMinimumSize( s.width() + verticalScrollBar()->sizeHint().width() +
		    lineWidth() * 2, s.height() );
  }
}

QSize CListView::sizeHint( void ) const
{
  QSize s = Q3ListView::sizeHint();

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
    pix = new QPixmap( SmallIcon( iconName ) );

    if( drawBorder == true )
    {
      //
      // 2000-01-23 Espen Sand
      // Careful here: If the mask has not been defined we can
      // not use QPixmap::mask() because it returns 0 => segfault
      //
      if( !pix->mask().isNull() )
      {
	QBitmap *bm = new QBitmap((pix->mask()));
	if( bm != 0 )
	{
	  QPainter qp(bm);
	  qp.setPen(QPen(Qt::white,1));
	  qp.drawRect(0,0,bm->width(),bm->height());
	  qp.end();
	  pix->setMask(*bm);
	}

	QPainter qp(pix);
	qp.setPen(QPen(Qt::red,1));
	qp.drawRect(0,0,pix->width(),pix->height());
	qp.end();
        delete bm;

      }
    }
    mPixDict.replace( iconName, pix );
  }

  return( *pix );
}


#include "listview.moc"




