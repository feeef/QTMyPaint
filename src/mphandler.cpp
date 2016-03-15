/*
    Copyright © 2015 by The QTMyPaint Project

    This file is part of QTMyPaint, a Qt-based interface for MyPaint C++ library.

    QTMyPaint is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QTMyPaint is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QTMyPaint. If not, see <http://www.gnu.org/licenses/>.
*/
#include "mphandler.h"

#include "mypaint-brush.h"
#include "mypaint-surface.h"

#include "libmypaint.c"

bool MPHandler::instanceFlag = false;
MPHandler* MPHandler::currentHandler = NULL;

static void
onUpdatedTile(MPSurface *surface, MPTile *tile)
{
    MPHandler* handler = MPHandler::handler();
    handler->requestUpdateTile(surface, tile);
}

static void
onNewTile(MPSurface *surface, MPTile *tile)
{
    MPHandler* handler = MPHandler::handler();
    handler->hasNewTile(surface, tile);
}


MPHandler *
MPHandler::handler()
{
    if(! instanceFlag)
    {
        currentHandler = new MPHandler();
        instanceFlag = true;
    }

    return currentHandler;

}

MPHandler::MPHandler()
{
    m_brush = new MPBrush();
    m_surface = new MPSurface(1920, 1080);

    this->m_surface->setOnUpdateTile(onUpdatedTile);
    this->m_surface->setOnNewTile(onNewTile);
}

MPHandler::~MPHandler()
{
    mypaint_surface_unref((MyPaintSurface *)m_surface);
}

void
MPHandler::requestUpdateTile(MPSurface *surface, MPTile *tile)
{
    emit updateTile(surface, tile);
}

void
MPHandler::hasNewTile(MPSurface *surface, MPTile *tile)
{
    emit newTile(surface, tile);
}

void
MPHandler::strokeTo(float x, float y, float pressure, float xtilt, float ytilt)
{
    float dtime = 1.0/10;
    mypaint_surface_begin_atomic((MyPaintSurface *)m_surface);
    mypaint_brush_stroke_to(m_brush->brush, (MyPaintSurface *)m_surface, x, y, pressure, xtilt, ytilt, dtime);
    MyPaintRectangle roi;
    mypaint_surface_end_atomic((MyPaintSurface *)m_surface, &roi);
}

void
MPHandler::startStroke()
{
    mypaint_brush_reset (m_brush->brush);
    mypaint_brush_new_stroke(m_brush->brush);
}

void
MPHandler::strokeTo(float x, float y)
{
    float pressure = 1.0;
    float xtilt = 0.0;
    float ytilt = 0.0;
    strokeTo(x, y, pressure, xtilt, ytilt);
}

void
MPHandler::endStroke()
{

}

void
MPHandler::setBrushColor(QColor newColor)
{
    this->m_brush->setColor(newColor);
}