/*
 * kdfconfig.cpp
 *
 * $Id$
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <math.h>

#include <qstring.h>
#include <qfile.h>
#include <qtstream.h>
#include <qstring.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpaintd.h>
#include <qlineedit.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qscrollbar.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <ktablistbox.h>
#include "kdfconfig.h"

#define BLANK ' '
#define DELIMITER '#'
#define BORDER 5
#define PIX_COLUMN 7
#define FULL_PERCENT 95.0
#define VISIBLE i18n("visible")
#define INVISIBLE i18n("invisible")

#define DEFAULT_FREQ 60
#define DEFAULT_FILEMGR_COMMAND "kfmclient openURL %m"
//#define DEFAULT_FILEMGR_COMMAND "kvt -e mc %m"

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif

/***************************************************************************
  * Constructor
**/
KDFConfigWidget::KDFConfigWidget (QWidget * parent, const char *name
                      , bool init)
    : KConfigWidget (parent, name)
{
  debug("Construct: KDFConfigWidget::KDFConfigWidget");


  tabWidths.resize(PIX_COLUMN+1);
  tabHeaders.append(i18n("Icon") );
  tabWidths[0]=32;
  tabHeaders.append(i18n("Device") );
  tabWidths[1]=80;
  tabHeaders.append(i18n("Type") );
  tabWidths[2]=50;
  tabHeaders.append(i18n("Size") );
  tabWidths[3]=72;
  tabHeaders.append(i18n("MountPoint") );
  tabWidths[4]=90;
  tabHeaders.append(i18n("Free") );
  tabWidths[5]=55;
  tabHeaders.append(i18n("Full%") );
  tabWidths[6]=70;
  tabHeaders.append(i18n("UsageBar") );
  tabWidths[7]=100;

  if (init) {
    GUI = FALSE;
  } else
    GUI = TRUE;

  if (GUI)
    {  // inits go here
      this->setMinimumSize(325,200);
      KIconLoader *loader = KGlobal::iconLoader();

      freqLabel = new QLabel( i18n("update frequency (seconds)")
                              ,this); CHECK_PTR(freqLabel);
      fileMgrLabel = new QLabel( i18n("FileManager (e.g. kvt -e mc %m)")
                              ,this); CHECK_PTR(fileMgrLabel);
      freqScroll = new QScrollBar( this ); CHECK_PTR(freqScroll); 
      freqScroll->setOrientation( QScrollBar::Horizontal );
      freqScroll->setSteps(1,20);
      freqScroll->setRange(0, 180 ); 
      freqScroll->setValue(DEFAULT_FREQ);


      freqLCD = new QLCDNumber( this ); CHECK_PTR(freqLCD);
      freqLCD->setNumDigits( 3 );
      freqLCD->setSegmentStyle(QLCDNumber::Filled);
      connect(freqScroll, SIGNAL(valueChanged(int)), 
              freqLCD, SLOT(display(int)) );
      fileMgrEdit = new QLineEdit( this ); CHECK_PTR(fileMgrEdit);
      
      cbOpenFileMgrOnMount = new QCheckBox(i18n("Open the above filemanager on mount"),this); 
      CHECK_PTR(cbOpenFileMgrOnMount);

      cbPopupIfFull = new QCheckBox(i18n("Pop up a window when a disk gets critically full"),this); 
      CHECK_PTR(cbPopupIfFull);
 
      QString pcn=parent->className();
      debug("parent: [%s]",pcn.latin1());
      if ( pcn == "KDFTopLevel" ) {
         //it is used from KTMainWindow "kdf" --> provides a help-btn
         isTopLevel=TRUE;  
      } else 
         isTopLevel=FALSE;


   // config-tabList ****************************************

      confTabList = new KTabListBox(this,"confTabList",PIX_COLUMN+1,0); 
      CHECK_PTR(confTabList);
      for (int i=0;i<=PIX_COLUMN;i++)
        confTabList->setColumn(i,tabHeaders.at(i)
                         ,tabWidths[i],KTabListBox::PixmapColumn);
      confTabList->setSeparator(BLANK);
      confTabList->dict().setAutoDelete(TRUE);
      QPixmap *pix;
      pix = new QPixmap(loader->loadApplicationMiniIcon("tick.xpm"));
      confTabList->dict().replace(VISIBLE,pix );
      pix = new QPixmap(loader->loadApplicationMiniIcon("delete.xpm"));
      confTabList->dict().replace(INVISIBLE,pix );
      confTabList->show();
    
      QString s;
      for (int i=0;i<=PIX_COLUMN;i++)
         s = s + i18n(VISIBLE) + " ";
      confTabList->appendItem((const char *)s);
      connect(confTabList,SIGNAL(headerClicked(int))
             ,this,SLOT(toggleColumnVisibility(int)) );
      connect(confTabList,SIGNAL(popupMenu(int,int))
             ,this,SLOT(toggleColumnVisibility(int,int)) );
      connect(confTabList,SIGNAL(selected(int,int))
             ,this,SLOT(toggleColumnVisibility(int,int)) );
      connect(confTabList,SIGNAL(midClick(int,int))
             ,this,SLOT(toggleColumnVisibility(int,int)) );
      connect(confTabList,SIGNAL(highlighted(int,int)) 
           ,this,SLOT(toggleColumnVisibility(int,int)) );
 
   }//if GUI

   config = kapp->getConfig();
   loadSettings();
   if (init)
     applySettings();
} // Constructor


/***************************************************************************
  * Destructor
**/
KDFConfigWidget::~KDFConfigWidget() 
{ 
  debug("DESTRUCT: KDFConfigWidget::~KDFConfigWidget");
  if (GUI) {
   delete freqLabel;
   delete fileMgrLabel;
   delete freqScroll;
   delete freqLCD;
   delete cbOpenFileMgrOnMount;
   delete cbPopupIfFull;
   delete confTabList;
  }
}; 

/***************************************************************************
  * is called when the program is exiting
**/
void KDFConfigWidget::closeEvent(QCloseEvent *)
{
  debug("KDFConfigWidget::closeEvent");
  applySettings(); 
  kapp->quit();
};

/***************************************************************************
  * saves KConfig and starts the df-cmd
**/
void KDFConfigWidget::applySettings()
{
  debug("KDFConfigWidget::applySettings");
 config->setGroup("KDFConfig");
 if (GUI) {
    config->writeEntry("Width",width() );
    config->writeEntry("Height",height() );
    config->writeEntry("UpdateFrequency",freqScroll->value());
    config->writeEntry("FileManagerCommand",fileMgrEdit->text() );
    config->writeEntry("PopupIfFull",cbPopupIfFull->isChecked());
    config->writeEntry("OpenFileMgrOnMount",cbOpenFileMgrOnMount->isChecked());
    for (int i=0;i<PIX_COLUMN;i++)
      config->writeEntry(tabHeaders.at(i),(confTabList->text(0,i)==VISIBLE));
 } else { //init
    config->writeEntry("FileManagerCommand",DEFAULT_FILEMGR_COMMAND );
 }//if GUI
 config->sync();
}


/***************************************************************************
  * reads the KConfig
**/
void KDFConfigWidget::loadSettings()
{
  debug("KDFConfigWidget::loadSettings");
 config->setGroup("KDFConfig");

 if (GUI) {
    if (isTopLevel) { //only "kdf" can be resized
       int appWidth=config->readNumEntry("Width",width());
       int appHeight=config->readNumEntry("Height",height());
       this->resize(appWidth,appHeight);
    }
    freqScroll->setValue(config->readNumEntry("UpdateFrequency",DEFAULT_FREQ));
    cbPopupIfFull->setChecked(config->readBoolEntry("PopupIfFull",TRUE));
    cbOpenFileMgrOnMount->setChecked(config->readBoolEntry("OpenFileMgrOnMount",FALSE));
    freqLCD->display(freqScroll->value());
    fileMgrEdit->setText(config->readEntry("FileManagerCommand"
                         ,DEFAULT_FILEMGR_COMMAND));
    for (int i=0;i<=PIX_COLUMN;i++) {
      int j=config->readNumEntry(tabHeaders.at(i),tabWidths[i]);
      if (j != 0)
        confTabList->changeItemPart(i18n(VISIBLE),0,i); 
      else
        confTabList->changeItemPart(i18n(INVISIBLE),0,i); 
   
    }//for
 }//if GUI
}

/***************************************************************************
  * resets all to its defaults
**/
void KDFConfigWidget::defaultsBtnClicked()
{
  debug("KDFConfigWidget::defaultsBtnClicked");
  freqScroll->setValue(DEFAULT_FREQ);
  fileMgrEdit->setText(DEFAULT_FILEMGR_COMMAND);
  for (int i=0;i<=PIX_COLUMN;i++) {
       confTabList->changeItemPart(i18n(VISIBLE),0,i); 
  }//for
  this->update();
}


/**************************************************************************
  * connected with the confTabList-Signal
**/
void KDFConfigWidget::toggleColumnVisibility(int column)
{
  debug("KDFConfigWidget::toggleColumnVisibility: %d",column);

  if(confTabList->text(0,column) == i18n(INVISIBLE) ) {
    confTabList->changeItemPart(i18n(VISIBLE),0,column); 
  } else {
    confTabList->changeItemPart(i18n(INVISIBLE),0,column); 
  }
  //confTabList->unmarkAll();
  this->update();
}


/**************************************************************************
  * calculates the sizes of the settings and the device-tabList
**/
void KDFConfigWidget::resizeEvent(QResizeEvent *)
{
  debug("KDFConfigWidget::resizeEvent  %dx%d",width(),height());
  
  //set widths to default-values
  for (int i=0;i<=PIX_COLUMN;i++)
      confTabList->setColumnWidth(i,tabWidths[i]);
  //same with confTabList
  int totalWidth=0;
  for (int i=0;i<PIX_COLUMN;i++) //all except the last (pix) column
    totalWidth += confTabList->columnWidth(i);
  //adjust size to fit completely into tabList-widget...
  if (confTabList->width() > totalWidth+4)
     confTabList->setColumnWidth(PIX_COLUMN
                        ,confTabList->width()-totalWidth-4);

   confTabList->setGeometry(0,0,this->width()-BORDER,60);

   freqLCD->setGeometry(5,confTabList->y()+confTabList->height()+BORDER,52,32);
 {
   int x=freqLCD->x()+freqLCD->width()+BORDER;
   int w=160;
   w=this->width() - x - 2*BORDER;
   freqScroll->setGeometry(x,freqLCD->y()+freqLCD->height()-16,w,16);
 }

  freqLabel->setGeometry(freqLCD->x()+freqLCD->width()+BORDER
                  ,freqLCD->y()
		  ,freqScroll->width()+3
                  ,freqLCD->height()-freqScroll->height() );

  fileMgrLabel->setGeometry(freqLCD->x()
                  ,freqScroll->y()+freqScroll->height()+BORDER
                  ,freqScroll->width()+freqLCD->width()+BORDER
                  ,16 );

  fileMgrEdit->setGeometry(fileMgrLabel->x()
                 ,(fileMgrLabel->y()+fileMgrLabel->height())
                 ,fileMgrLabel->width()
                 ,25 );

  cbOpenFileMgrOnMount->setGeometry(fileMgrEdit->x()
              ,fileMgrEdit->y()+fileMgrEdit->height()+3,
              fileMgrEdit->width(),fileMgrEdit->height());

  cbPopupIfFull->setGeometry(cbOpenFileMgrOnMount->x()
              ,cbOpenFileMgrOnMount->y()+cbOpenFileMgrOnMount->height()+3,
              cbOpenFileMgrOnMount->width(),cbOpenFileMgrOnMount->height());
                 
  // repaint();
}

#include "kdfconfig.moc"
