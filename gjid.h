// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "level.h"
#include "xapp.h"

//----------------------------------------------------------------------

class GJID : public CXApp {
    enum EGameState {
	state_Title,
	state_Story,
	state_Game,
	state_Winner,
	state_Loser,
	state_Last
    };
public:
    static GJID&	Instance (void)	{ static GJID s_App; return (s_App); }
    int			Run (void);
protected:
			GJID (void);
    virtual void	OnDraw (void);
    virtual void	OnKey (key_t key);
private:
    inline void		PutTile (PicIndex tidx, int x, int y)	{ DrawImageTile (_imgtiles, c_Tiles[tidx], x, y); }
    inline void		GoToState (EGameState state)		{ _state = state; Update(); }
    void		FillWithTile (PicIndex tidx);
    void		DecodeBitmapWithTile (const uint8_t* p, PicIndex fg, PicIndex bg);
    inline void		PrintStory (void);
    inline void		DrawLevel (void);
    inline void		TitleKeys (key_t key);
    inline void		StoryKeys (key_t key);
    inline void		LevelKeys (key_t key);
private:
    EGameState		_state;
    uint32_t		_storyPage;
    uint32_t		_level;
    SImage		_imgtiles;
    SImage		_imglogo;
    Level		_curLevel;
    vector<Level>	_levels;
    static const SImageTile c_Tiles [NumberOfPics];
};
