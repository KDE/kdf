/*
 * mntconfig.cpp
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
#include <qmsgbox.h> 
#include <qfile.h>
#include <qtstream.h>
#include <qstring.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpaintd.h>

#include <kapp.h> 
#include <kmsgbox.h> 
#include <kiconloader.h>
#include <kiconloaderdialog.h>
#include <kfiledialog.h>
#include "mntconfig.h"
#include "mntconfig.moc"


static bool GUI;
#define NRCOLS 5
#define ICONCOL 0
#define DEVCOL 1
#define MNTPNTCOL 2
#define MNTCMDCOL 3
#define UMNTCMDCOL 4

/***************************************************************************
  * Constructor
**/
MntConfigWidget::MntConfigWidget (QWidget * parent, const char *name
                      , bool init)
    : KConfigWidget (parent, name)
{
  //debug("Construct: MntConfigWidget::MntConfigWidget");
  if (init) {
    GUI = FALSE;
  } else
    GUI = TRUE;
  tabWidths.resize(NRCOLS);
  tabHeaders.append(klocale->translate("Icon") );
  tabWidths[0]=32;
  tabHeaders.append(klocale->translate("Device") );
  tabWidths[1]=80;
  tabHeaders.append(klocale->translate("MountPoint") );
  tabWidths[2]=90;
  tabHeaders.append(klocale->translate("MountCommand") );
  tabWidths[3]=120;
  tabHeaders.append(klocale->translate("UmountCommand") );
  tabWidths[4]=120;
  actRow=-1;
  actDisk=0;

  if (GUI)
    {  // inits go here
       diskList.readFSTAB();
       diskList.readDF();
       //tabList fillup waits until disklist.readDF() is done...
       initializing=TRUE;
       loader = kapp->getIconLoader(); CHECK_PTR(loader);
       connect(&diskList,SIGNAL(readDFDone()),this,SLOT(readDFDone()));
       tabList=new KTabListBox(this,"tabList",NRCOLS,0); CHECK_PTR(tabList);
       tabList->setSeparator('\t');
       tabList->setColumn(0,tabHeaders.at(0)
                         ,tabWidths[0]
                         ,KTabListBox::PixmapColumn);
       for (int i=1;i<NRCOLS;i++)
       tabList->setColumn(i,tabHeaders.at(i)
                         ,tabWidths[i],KTabListBox::TextColumn);
       boxActDev=new QGroupBox(this); CHECK_PTR(boxActDev);
       boxActDev->setEnabled(FALSE);
       QString title;
       title.sprintf("%s [%s] %s [%s]",tabHeaders.at(1),"NONE"
                                      ,tabHeaders.at(2),"NONE");
       boxActDev->setTitle(title.data());
 
       btnActIcon=new QPushButton(boxActDev); CHECK_PTR(btnActIcon);
       btnActIcon->setEnabled(FALSE);
       connect(btnActIcon,SIGNAL(clicked()),this,SLOT(selectIcon()));
       qleIcon=new QLineEdit(boxActDev); CHECK_PTR(qleIcon);
       connect(qleIcon,SIGNAL(textChanged(const char *))
               ,this,SLOT(iconChanged(const char *)));
       qleIcon->setEnabled(FALSE);

       //Mount
       qleMnt=new QLineEdit(boxActDev); CHECK_PTR(qleMnt);
       connect(qleMnt,SIGNAL(textChanged(const char *))
               ,this,SLOT(mntCmdChanged(const char *)));
       qleMnt->setEnabled(FALSE);
       btnMntFile=new QPushButton(klocale->translate("get &MountCommand")
                  ,boxActDev); CHECK_PTR(btnMntFile);
       connect(btnMntFile,SIGNAL(clicked()),this,SLOT(selectMntFile()));
       btnMntFile->setEnabled(FALSE);

       //Umount
       qleUmnt=new QLineEdit(boxActDev); CHECK_PTR(qleUmnt);
       connect(qleUmnt,SIGNAL(textChanged(const char *))
               ,this,SLOT(umntCmdChanged(const char *)));
       qleUmnt->setEnabled(FALSE);
       btnUmntFile=new QPushButton(klocale->translate("get &UmountCommand")
                  ,boxActDev); CHECK_PTR(btnUmntFile);
       connect(btnUmntFile,SIGNAL(clicked()),this,SLOT(selectUmntFile()));
       btnUmntFile->setEnabled(FALSE);

   }//if GUI
 config = kapp->getConfig();
 loadSettings();
 if (init) applySettings();
} // Constructor


/**********************************************************************/
void MntConfigWidget::readDFDone() {
  //debug("MntConfigWidget::readDFDone");
  initializing=FALSE;
  tabList->clear();
       //fill up the tabListBox
       DiskEntry *disk;
       //      KIconLoader *loader = kapp->getIconLoader();
       QPixmap *pix;
       QString s,icon;
       for (disk=diskList.first();disk!=0;disk=diskList.next()) {
        icon.sprintf("%s%s%s",disk->iconName().data()
                             ,disk->deviceName().data()
                             ,disk->mountPoint().data());
         s.sprintf("%s\t%s\t%s\t%s\t%s",icon.data()
                                   ,disk->deviceName().data()
                                   ,disk->mountPoint().data()
                                   ,disk->mountCommand().data()
                                   ,disk->umountCommand().data());
         tabList->appendItem((const char *)s);
       pix=tabList->dict()[icon.data()];
       if (pix == 0) { // pix not already in cache
          pix = new QPixmap(loader->loadMiniIcon(disk->iconName()));
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
         tabList->dict().replace(icon.data(),pix );
       }
       }//for every disk
       tabList->show();
       connect(tabList,SIGNAL(highlighted(int,int)),this,SLOT(clicked(int,int)));
       connect(tabList,SIGNAL(selected(int,int)),this,SLOT(clicked(int,int)));
       connect(tabList,SIGNAL(midClick(int,int)),this,SLOT(clicked(int,int)));
       connect(tabList,SIGNAL(popupMenu(int,int)),this,SLOT(clicked(int,int)));

   loadSettings();
   applySettings();
}


/***************************************************************************
  * saves KConfig and starts the df-cmd
**/
void MntConfigWidget::applySettings()
{
  //debug("MntConfigWidget::applySettings");
 diskList.applySettings();
 config->setGroup("MntConfig");
 if (GUI) {
   config->writeEntry("Width",this->width() );
   config->writeEntry("Height",this->height() );
 }
 config->sync();
}

/***************************************************************************
  * reads the KConfig
**/
void MntConfigWidget::loadSettings()
{
  //debug("MntConfigWidget::loadSettings");
 if ((!initializing) && (GUI)) {
       config->setGroup("MntConfig");
       if (isTopLevel()) {
         int w=config->readNumEntry("Width",this->width() );
         int h=config->readNumEntry("Height",this->height() );
         this->resize(w,h);
       }
       
    //renew the views (qle etc)
   if (qleMnt->hasFocus())
     this->clicked(actRow,MNTCMDCOL);
   else if (qleUmnt->hasFocus())
     this->clicked(actRow,UMNTCMDCOL);
   else this->clicked(actRow,DEVCOL);
       
 }//if not initializing and GUI
  
}

/***************************************************************************
  * is invoked when you click on an entry in the list
**/
void MntConfigWidget::clicked(int index, int column)
{
  //debug("MntConfigWidget::clicked row %i column %i",index, column);
  if ((index == -1) || (tabList->count()<(uint)index)) return;
  actRow=index;
  tabList->unmarkAll();
  tabList->markItem(actRow);
  boxActDev->setEnabled(TRUE);
  btnActIcon->setEnabled(TRUE);
  qleIcon->setEnabled(TRUE);
  qleMnt->setEnabled(TRUE);
  qleUmnt->setEnabled(TRUE);
  btnMntFile->setEnabled(TRUE);
  btnUmntFile->setEnabled(TRUE);
  btnUmntFile->setEnabled(TRUE);
  if (column==MNTCMDCOL) qleMnt->setFocus();
  if (column==UMNTCMDCOL) qleUmnt->setFocus();
  DiskEntry *disk = new DiskEntry();
  disk->setDeviceName(tabList->text(index,DEVCOL));
  disk->setMountPoint(tabList->text(index,MNTPNTCOL));
  actDisk=diskList.at(diskList.find(disk));
  delete disk;
  QString title;
  title.sprintf("%s [%s] %s [%s]",tabHeaders.at(DEVCOL)
                                 ,actDisk->deviceName().data()
                                 ,tabHeaders.at(MNTPNTCOL)
                                 ,actDisk->mountPoint().data());
  QString icon;
         icon.sprintf("%s%s%s",actDisk->iconName().data()
                            ,actDisk->deviceName().data()
                            ,actDisk->mountPoint().data());

  boxActDev->setTitle(title.data());
  //   KIconLoader *loader = kapp->getIconLoader();
  btnActIcon->setPixmap(*(tabList->dict()[icon.data()]));
  qleIcon->setText(actDisk->iconName().left(actDisk->iconName().findRev('_')) );
  qleMnt->setText(actDisk->mountCommand().data());
  qleUmnt->setText(actDisk->umountCommand().data());

}


void MntConfigWidget::selectIcon()
{
  // KIconLoader *loader = kapp->getIconLoader();
  KIconLoaderDialog *kild=new KIconLoaderDialog(loader,this);
  CHECK_PTR(kild);
  QStrList dirs;

  dirs.append(kapp->kde_icondir() + "/mini");
  dirs.append(KApplication::localkdedir()+"/share/icons/mini");

  kild->setDir(&dirs);
  QString icoName;
  QPixmap *pix=new QPixmap(kild->selectIcon(icoName,"*"));
  delete pix;
  delete kild;
  if (!icoName.isEmpty()) {
    if ( (icoName.findRev('_')==0) || //file starts with a _
          ((icoName.right(icoName.length()-icoName.findRev('_'))
                        != "_mount.xpm") &&
          (icoName.right(icoName.length()-icoName.findRev('_'))
                        != "_unmount.xpm") ) )
       KMsgBox::message(this,klocale->translate("KDiskFree"),
              klocale->translate("This filename is not valid.\nIt has to be ending in \n\"_mount.xpm\" or \"_unmount.xpm\".")
               ,KMsgBox::EXCLAMATION);
     else {
       icoName=icoName.left(icoName.findRev('_'));
       actDisk->setIconName(icoName);
  QString icon;
       icon.sprintf("%s%s%s",actDisk->iconName().data()
                            ,actDisk->deviceName().data()
                            ,actDisk->mountPoint().data());

       pix=tabList->dict()[icon.data()];
       if (pix == 0) { // pix not already in cache
          pix = new QPixmap(loader->loadMiniIcon(actDisk->iconName()));
          if ( -1==actDisk->mountOptions().find("user",0,FALSE) ) {
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
         tabList->dict().replace(icon.data(),pix );
       }
     btnActIcon->setPixmap(*pix);
     qleIcon->setText(icoName.data());
     tabList->changeItemPart(icon.data(),actRow,ICONCOL);
     //     tabList->repaint();
     }
  };

}

void MntConfigWidget::selectMntFile()
{
  QString cmd=KFileDialog::getOpenFileName("","*",this);
  if (!cmd.isEmpty()) qleMnt->setText(cmd.data());
}

void MntConfigWidget::selectUmntFile()
{
  QString cmd=KFileDialog::getOpenFileName("","*",this);
  if (!cmd.isEmpty()) qleUmnt->setText(cmd.data());
}

void MntConfigWidget::iconChanged(const char * data)
{
}

void MntConfigWidget::mntCmdChanged(const char * data)
{
  tabList->changeItemPart(data,actRow,MNTCMDCOL);
  actDisk->setMountCommand(data);
}

void MntConfigWidget::umntCmdChanged(const char * data)
{
  tabList->changeItemPart(data,actRow,UMNTCMDCOL);
  actDisk->setUmountCommand(data);
}

/***************************************************************************
  * Destructor
**/
MntConfigWidget::~MntConfigWidget() 
{ 
  //debug("DESTRUCT: MntConfigWidget::~MntConfigWidget");
  if (GUI) {
    delete btnActIcon;
    delete qleIcon;
    delete qleMnt;
    delete btnMntFile;
    delete qleUmnt;
    delete btnUmntFile;

    delete boxActDev;
    delete tabList;
   
  }
}; 

/***************************************************************************
  * is called when the program is exiting
**/
void MntConfigWidget::closeEvent(QCloseEvent *)
{
  //debug("MntConfigWidget::closeEvent");
  //applySettings(); 
  // kapp->quit();
};

/**************************************************************************
  * calculates the sizes of the settings and the device-tabList
**/
void MntConfigWidget::resizeEvent(QResizeEvent *)
{
  boxActDev->setGeometry(0,this->height()-80,this->width(),80);
  
  tabList->setGeometry(0,0,this->width(),boxActDev->y());
  int frontCols=0;
  for(int i=0;i<=MNTPNTCOL;i++)
     frontCols+=tabList->columnWidth(i);

  //mount and umount col is same width  (range: 0..NRCOLS-1)
  int col=(this->width()-4-frontCols)/2;
  tabList->setColumnWidth(MNTCMDCOL,col);
  tabList->setColumnWidth(UMNTCMDCOL,col);

  qleMnt->setGeometry(frontCols,20,col,25);
  btnMntFile->setGeometry(qleMnt->x(),qleMnt->y()+qleMnt->height()+5
                         ,qleMnt->width(),qleMnt->height());
  btnActIcon->setGeometry(btnMntFile->x()-qleMnt->height()-5
                         ,btnMntFile->y(),qleMnt->height()
                         ,qleMnt->height());
  qleIcon->setGeometry(5,btnActIcon->y()
                       ,btnActIcon->x()-5-5,btnActIcon->height());
  qleUmnt->setGeometry(frontCols+col,20,col,25);
  btnUmntFile->setGeometry(qleUmnt->x(),qleUmnt->y()+qleUmnt->height()+5
                          ,qleUmnt->width(),qleUmnt->height());

}

