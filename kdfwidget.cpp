/*
 * kdfwidget.cpp
 *
 * $Id$
 *
 * Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
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
#include <qmsgbox.h> 
#include <qfile.h>
#include <qtstream.h>
#include <qstring.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpaintd.h>

#include <kapp.h> 
#include <kglobal.h>
#include <kiconloader.h>
#include <ktablistbox.h>

#include "kdfwidget.h"

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
KDFWidget::KDFWidget (QWidget * parent, const char *name
                      , bool init)
    : KConfigWidget (parent, name)
{
  debug("Construct: KDFWidget::KDFWidget");

  popupIfFull=TRUE;
  openFileMgrOnMount=FALSE;
  updateFreq=DEFAULT_FREQ;
  connect(&diskList , SIGNAL(readDFDone() ),
           this, SLOT (updateDFDone()) );
  connect(&diskList , SIGNAL(criticallyFull(DiskEntry*)),
           this, SLOT (criticallyFull(DiskEntry*)) );

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
      //      this->setMinimumSize(323,100);
      KIconLoader *loader = KGlobal::iconLoader();
      
      //CONFIGURATION WINDOW
      tabconf=new QTabDialog(); CHECK_PTR(tabconf);
      mntconf=new MntConfigWidget(tabconf,"mntconf");CHECK_PTR(mntconf);
      kdfconf=new KDFConfigWidget(tabconf,"kdfconf");CHECK_PTR(kdfconf);

      QString pcn=parent->className();
      debug("parent: [%s]",pcn.latin1());
      if ( pcn == "KDFTopLevel" ) {
         //it is used from KTMainWindow "kdf"
         isTopLevel=TRUE;  
      } else 
         isTopLevel=FALSE;


   // normal tabList ****************************************

      tabList = new KTabListBox(this,"tabList",PIX_COLUMN+1,0); 
      CHECK_PTR(tabList);
      tabList->setColumn(0,tabHeaders.at(0)
                         ,tabWidths[0]
                         ,KTabListBox::PixmapColumn);
      for (int i=1;i<PIX_COLUMN;i++)
        tabList->setColumn(i,tabHeaders.at(i)
                         ,tabWidths[i],KTabListBox::TextColumn
                         ,KTabListBox::SimpleOrder);
      tabList->setColumn(PIX_COLUMN,tabHeaders.at(PIX_COLUMN)
                         ,tabWidths[PIX_COLUMN]
                         ,KTabListBox::PixmapColumn);
      QPixmap *pix;
      pix = new QPixmap(loader->loadApplicationMiniIcon("mini-clock.xpm"));
      tabList->dict().replace("WAIT",pix );

      tabList->setSeparator(BLANK);
      connect(tabList,SIGNAL(popupMenu(int,int))
             ,this,SLOT(popupMenu(int,int)) );
      connect(tabList,SIGNAL(selected(int,int))
             ,this,SLOT(popupMenu(int,int)) );
      connect(tabList,SIGNAL(midClick(int,int))
             ,this,SLOT(popupMenu(int,int)) );
      connect(tabList,SIGNAL(highlighted(int,int))
           ,this,SLOT(popupMenu(int,int)) );
      tabList->dict().setAutoDelete(TRUE);
   }//if GUI

   config = kapp->getConfig();
   loadSettings();
   if (init)
     applySettings();
} // Constructor


/***************************************************************************
  * Destructor
**/
KDFWidget::~KDFWidget() 
{ 
  debug("DESTRUCT: KDFWidget::~KDFWidget");
  if (GUI) {
  }
}; 

/***************************************************************************
  * is called when the program is exiting
**/
void KDFWidget::closeEvent(QCloseEvent *)
{
  debug("quitting KDFWidget");
  applySettings(); 
  kapp->quit();
};

/***************************************************************************
  * saves KConfig and starts the df-cmd
**/
void KDFWidget::applySettings()
{
  debug("KDFWidget::applySettings");
 config->setGroup("KDiskFree");
 if (isTopLevel) { // only "kdf" is resizable
   config->writeEntry("Width",width() );
   config->writeEntry("Height",height() );
 }
 config->sync();

 if (GUI) {
   for (int i=0;i<=PIX_COLUMN;i++)
      config->writeEntry(tabHeaders.at(i),tabList->columnWidth(i) );
   config->setGroup("KDFConfigTab");
   config->writeEntry("Width",tabconf->width() );
   config->writeEntry("Height",tabconf->height() );
 }
   updateDF();
}

void KDFWidget::confApplySettings()
{
  // debug("KDFWidget::confApplySettings");
 this->applySettings(); //store ev. changes in tabWidths
 kdfconf->applySettings();
 mntconf->applySettings();
 this->loadSettings();  //get the new changes
}

/***************************************************************************
  * reads the KConfig
**/
void KDFWidget::loadSettings()
{
  debug("KDFWidget::loadSettings");
 config->setGroup("KDFConfig");
 updateFreq=config->readNumEntry("UpdateFrequency",DEFAULT_FREQ);
 fileMgr=config->readEntry("FileManagerCommand",DEFAULT_FILEMGR_COMMAND);
 popupIfFull=config->readBoolEntry("PopupIfFull",popupIfFull);
openFileMgrOnMount=config->readBoolEntry("OpenFileMgrOnMount",openFileMgrOnMount);

 if (GUI) {
    config->setGroup("KDFConfigTab");
    int appWidth=config->readNumEntry("Width",width());
    int appHeight=config->readNumEntry("Height",height());
    tabconf->resize(appWidth,appHeight);

    if (isTopLevel) { //only "kdf" can be resized
       config->setGroup("KDiskFree");
       int appWidth=config->readNumEntry("Width",width());
       int appHeight=config->readNumEntry("Height",height());
       this->resize(appWidth,appHeight);
    }
    for (int i=0;i<=PIX_COLUMN;i++)
       tabList->setColumnWidth(i,config->readNumEntry(tabHeaders.at(i)
                                                  ,tabWidths[i]) );
    config->setGroup("KDFConfig");
    for (int i=0;i<=PIX_COLUMN;i++) 
      // if not visible
      if (config->readBoolEntry(tabHeaders.at(i),true) == false) {
	tabList->setColumnWidth(i,0);  // dont show
      } else {
        if (tabList->columnWidth(i)==0)
          tabList->setColumnWidth(i,tabWidths[i]);  // if visible but was 0
      }
  setUpdateFreq(updateFreq);
  updateDF();
 }//if GUI
}

void KDFWidget::confLoadSettings()
{
  debug("KDFWidget::confLoadSettings");
 mntconf->loadSettings();
 kdfconf->loadSettings();
}

/***************************************************************************
  * pops up the SettingsBox if the settingsBtn is clicked
**/
void KDFWidget::settingsBtnClicked()
{
  if (isTopLevel) {
    //DEL OLD CONFWINDOW
    delete kdfconf;
    delete mntconf;
    delete tabconf;

    //CONFIGURATION WINDOW
      tabconf=new QTabDialog(); CHECK_PTR(tabconf);
      tabconf->setCaption(i18n("KDiskFree/KwikDisk Configuration"));

      mntconf=new MntConfigWidget(tabconf,"mntconf");CHECK_PTR(mntconf);
      kdfconf=new KDFConfigWidget(tabconf,"kdfconf");CHECK_PTR(kdfconf);
      kdfconf->setMinimumSize(460,200);
      tabconf->setApplyButton(i18n("&Apply"));
      connect(tabconf,SIGNAL(applyButtonPressed()),
            this,SLOT(confApplySettings()));
      tabconf->setCancelButton(i18n("&Cancel"));
      connect(tabconf,SIGNAL(cancelButtonPressed()),
            this,SLOT(confLoadSettings()));
      tabconf->addTab(kdfconf,i18n("&General Settings"));
      tabconf->addTab(mntconf,i18n("(U)&MountCommands"));
      tabconf->setMinimumSize(460,200);
   
     tabconf->show();
     updateDF();     
     //     mntconf->loadSettings();  
  }
  applySettings();
  repaint();
}


/***************************************************************************
  * resets the timer for automatic df-refreshes
**/
void KDFWidget::setUpdateFreq(int freq)
{
  debug("KDFWidget::setUpdateFreq");
  killTimers(); //kills !all! running timers
  updateFreq=freq;
  if (updateFreq > 0)  //0 sets to NO_AUTO_UPDATE   ;)
    startTimer( updateFreq * 1000 );
  applySettings();
}

/***************************************************************************
  * Update (reread) all disk-dependencies
**/
void KDFWidget::timerEvent(QTimerEvent *) 
{ 
  debug("KDFWidget::timerEvent");
  updateDF();
};

/***************************************************************************
  * checks fstab & df 
**/
void KDFWidget::updateDF()
{
  readingDF=TRUE;
  diskList.readFSTAB();
  diskList.readDF(); 
};

/***************************************************************************
  * gets the signal when the diskList is complete and up to date
**/
void KDFWidget::updateDFDone()
{
  debug("KDFWidget::updateDFdone()");
  tabList->setAutoUpdate(FALSE);
  tabList->clear();
  DiskEntry *disk;
  QString s,percS,sizeS, icon, fullbar;
  KIconLoader *loader = KGlobal::iconLoader();
  QPixmap *pix;
  for (disk=diskList.first();disk!=0;disk=diskList.next()) {
       if ( disk->kBSize() > 0 ) {
         percS.sprintf("%2.1f%%",disk->percentFull() );
         sizeS=disk->prettyKBSize();
       } else {
         sizeS=i18n("UNKNOWN");    
         percS=i18n("UNKNOWN");    
       }
       icon.sprintf("%s%s%s",disk->iconName().latin1()
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
       fullbar.sprintf("%s%s%s","BAR"
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
    s.sprintf("%s %s %s %s %s %s %s %s"
                          ,(const char *)icon
                          ,(const char *)disk->deviceName()
                          ,(const char *)disk->fsType()
                          ,(const char *)sizeS
                          ,(const char *)disk->mountPoint()
                          ,(const char *)disk->prettyKBAvail()
                          ,(const char *)percS
                          ,(const char *)fullbar );
       //   debug("store [%s]", (const char *)s);
       tabList->appendItem((const char *)s);
       pix=tabList->dict()[icon.latin1()];
       if (pix == 0) { // pix not already in cache
          pix = new QPixmap(loader->loadApplicationMiniIcon(disk->iconName()));
          if ( -1==disk->mountOptions().find("user",0,FALSE) ) {
             // special root icon, normal user can´t mount
            QPainter *qp;
            QBitmap *bm=new QBitmap(*(pix->mask()));
            int w=1;  //width of the rect
            if (bm != 0) { //a mask exists, draw the rect on the mask first
              qp=new QPainter(bm);
              qp->setPen(QPen(white,w));
              qp->drawRect(0,0,bm->width(),bm->height());
              qp->end();
              pix->setMask(*bm);
            }
            qp=new QPainter(pix);
            qp->setPen(QPen(red,w));
            qp->drawRect(0,0,pix->width(),pix->height());
            qp->end();
         }
         tabList->dict().replace(icon.latin1(),pix );
       }
  }
  tabList->setAutoUpdate(TRUE);
  this->update();  //enables the automatic PIX_COLUMN-resize but flickers!
  updatePixmaps();
  readingDF=FALSE;
};
 
/**************************************************************************
  * connected with diskList
**/
void KDFWidget::criticallyFull(DiskEntry *disk )
{
  debug("KDFWidget::criticallyFull");
  if (popupIfFull) {
    QString s = i18n("Device [%1] on [%1] is getting critically full!").
      arg(disk->deviceName()).arg(disk->mountPoint());
    QMessageBox::warning(this,kapp->getCaption(), s, i18n("OK"));
  }
}

/**************************************************************************
  * connected with the confTabList-Signal
**/
void KDFWidget::toggleColumnVisibility(int column)
{
  debug("KDFWidget::toggleColumnVisibility: %d %d",column,tabList->columnWidth(column));

  if(tabList->columnWidth(column) == 0) {
    tabList->setColumnWidth(column,tabWidths[column]);
  } else {
    tabList->setColumnWidth(column,0);
  }
  this->update();
  updateDF();
}


/**************************************************************************
  * pops up and asks for mount/umount right-clicked device
**/
void KDFWidget::popupMenu(int row,int column)
{
  debug("KDFWidget::popupMenu: %d:%d",row,column);

  // get the clicked disk
      QString dev=tabList->text(row,1);
      QString mnt=tabList->text(row,4);
      DiskEntry *disk = new DiskEntry(dev);
      disk->setMountPoint(mnt);
      int pos=diskList.find(disk);
      delete disk;
      disk=diskList.at(pos);
  
      
  switch (column) { 
  case 0:   //mount/umount icon
  case 1:   //mount/umount deviceName  
    if (!readingDF) {
      unsigned int ipos=0;
      QString e1, e2;
      while(ipos<tabList->count()) {  //find old entry in tabList
         e1=tabList->text(ipos,1);
	 e2=tabList->text(ipos,4);
	 if (e1==(const char *)disk->deviceName()
	  && (e2==(const char *)disk->mountPoint())) {
                                //entry found
             tabList->markItem(ipos);
             tabList->changeItemColor(yellow,ipos);
             tabList->changeItemPart(i18n("WAIT"),ipos,0);
             tabList->changeItemPart(i18n("MOUNTING!"),ipos,3);
             tabList->changeItemPart(i18n("MOUNTING!"),ipos,5);
         }//if
         ipos++;
      }//while        
  
      if (!disk->toggleMount())
         QMessageBox::warning(this, kapp->getCaption(),
			      disk->lastSysError(), i18n("OK"));
      else 
        if ((openFileMgrOnMount) && (!disk->mounted())) popupMenu(row,4); 
      updateDF();
    }
      break;
  case 4:  //open fileManager on MountPoint
      QString cmdS;
      if ( !fileMgr.isEmpty() ) {
         cmdS=fileMgr;
         int pos=cmdS.find("%m");
         if ( pos > 0 ) {
             cmdS=cmdS.replace(pos,2
                           ,(const char *)disk->mountPoint() );
             cmdS.append(" &");
         } else //no %m found; just add path on the back
             cmdS=cmdS+" "+(const char *)disk->mountPoint()+" &";
         debug("fileMgr-cmd: [%s]",(const char *)cmdS);
  
	system(cmdS);
       }//if 
      break;
 }//switch
};

/**************************************************************************
  * recalculates and repaints the pixBars
**/
void KDFWidget::updatePixmaps()
{
  debug("KDFWidget::updatePixmaps");
  tabList->setAutoUpdate(FALSE);
  tabList->unmarkAll();
  for ( DiskEntry *disk=diskList.first(); disk != 0; disk=diskList.next() ) {
    if ( !disk->mounted() ) { 
       unsigned int ipos=0;
       QString e1, e2;
       while(ipos<tabList->count()) {  //find old entry in tabList
          e1=tabList->text(ipos,1);
	  e2=tabList->text(ipos,4);
	  if ((e1==(const char *)disk->deviceName())
	    && (e2==(const char *)disk->mountPoint())) {
              //entry found
	    //tabList->markItem(ipos);
             tabList->changeItemColor(lightGray,ipos);
             QString fullbar;
             fullbar.sprintf("%s%s%s","BAR"
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
             tabList->dict().remove((const char *)fullbar);
             tabList->changeItemPart("",ipos,PIX_COLUMN);
          }//if
        ipos++;
        }//while        
    }//if not mounted
    else if (disk->percentFull() != -1) { //always true for a mounted disk ?!
      if (disk->percentFull() > FULL_PERCENT) {
       unsigned int ipos=0;
       QString e1, e2;
       while(ipos<tabList->count()) {  //find old entry in tabList
          e1=tabList->text(ipos,1);
	  e2=tabList->text(ipos,4);
	  if ((e1==(const char *)disk->deviceName()) 
	    && (e2==(const char *)disk->mountPoint())) {
                                    //entry found
	    //tabList->markItem(ipos);
             tabList->changeItemColor(red,ipos);
          }//if
        ipos++;
        }//while        
      }//if full over FULL_PERCENT
      
      if (tabList->cellWidth(PIX_COLUMN) > 7) {
        QPixmap *pix;
        pix = new QPixmap(tabList->cellWidth(PIX_COLUMN)-7
                      , tabList->cellHeight(1)-1 );
        CHECK_PTR(pix);
        pix->fill(white );
        QPainter p(pix);
        p.setPen(black);
        p.drawRect(0,0,pix->width(),pix->height());
        if (disk->percentFull() > FULL_PERCENT)
            p.setBrush(red);
        else
            p.setBrush(darkBlue);
        p.setPen(white);
        p.drawRect(1,1
             ,(int)(  ((float)pix->width()-2) 
                     * (disk->percentFull()/100) )
             ,pix->height()-2 );
       QString fullbar;
       fullbar.sprintf("%s%s%s","BAR"
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
        tabList->dict().replace((const char *)fullbar, pix);
      }//if usageBar is visible
    }//was mounted
  }//for
  tabList->setAutoUpdate(TRUE);
  tabList->repaint();
}


/**************************************************************************
  * calculates the sizes and shows all devices
**/
void KDFWidget::paintEvent(QPaintEvent *)
{
  debug("KDFWidget::paintEvent");

  //resizing of PIX_COLUMN *************************************
  if (tabList->columnWidth(PIX_COLUMN) != 0 ){
    int totalWidth=0;
    for (int i=0;i<PIX_COLUMN;i++) //all except the last (pix) column
      totalWidth += tabList->columnWidth(i);
    //adjust size to fit completely into tabList-widget...
    if (tabList->width() > totalWidth-4)
       tabList->setColumnWidth(PIX_COLUMN,tabList->width()-totalWidth-4);
  }//if user hasn't turned of the usageBar

  updatePixmaps();
}

/**************************************************************************
  * calculates the sizes of the settings and the device-tabList
**/
void KDFWidget::resizeEvent(QResizeEvent *)
{
  debug("KDFWidget::resizeEvent  %dx%d",width(),height());
  int tabListHeight=this->height();
  int oldPixColumnWidth=tabList->columnWidth(PIX_COLUMN);
  tabList->setGeometry(0, 0, this->width()-1,tabListHeight);
       //this -1 is necessary....         ^^^^  god knows why?
      //setGeometry also changes the width of last column!!
  tabList->setColumnWidth(PIX_COLUMN,oldPixColumnWidth);
  debug("PIX_COLUMN_width=%d",tabList->columnWidth(PIX_COLUMN));

  repaint();
  applySettings();
}

void KDFWidget::invokeHTMLHelp()
{
 kapp->invokeHTMLHelp("kcontrol/kdf/index.html","");
}

#include "kdfwidget.moc"

