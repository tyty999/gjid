// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "level.h"
#include "font.h"
#include "xapp.h"

//----------------------------------------------------------------------

class GJID : public CXApp {
public:
    static GJID&	Instance (void);
protected:
			GJID (void);
    virtual void	OnDraw (CGC& gc);
    virtual void	OnKey (key_t key);
private:
    enum EGameState {
	state_Title,
	state_Story,
	state_Game,
	state_Winner,
	state_Loser,
	state_Last
    };
private:
    void		GoToState (EGameState state);
    void		FillWithTile (CGC& gc, PicIndex tidx);
    void		DecodeBitmapWithTile (CGC& gc, const uint16_t* p, size_t n, PicIndex tidx);
    inline void		IntroScreen (CGC& gc);
    inline void		LoserScreen (CGC& gc);
    inline void		PrintStory (CGC& gc);
    inline void		WinnerScreen (CGC& gc);
    inline void		DrawLevel (CGC& gc);
    inline void		TitleKeys (key_t key);
    inline void		StoryKeys (key_t key);
    inline void		LevelKeys (key_t key);
    void		LoadData (const char* filename);
    inline void		PutTile (PicIndex ti, int x, int y);
private:
    EGameState		_state;
    uoff_t		_storyPage;
    uoff_t		_level;
    SImage		_imgtiles;
    SImage		_imglogo;
    Level		_curLevel;
    CFont		_font;
    picvec_t		_pics;
    CPalette		_palette;
    vector<Level>	_levels;
};
