// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "level.h"
#include "xapp.h"

//----------------------------------------------------------------------

class GJID : public CXApp {
public:
    static GJID&	Instance (void);
protected:
			GJID (void);
    virtual void	OnDraw (void);
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
    void		FillWithTile (PicIndex tidx);
    void		DecodeBitmapWithTile (const uint16_t* p, size_t n, PicIndex tidx);
    inline void		IntroScreen (void);
    inline void		LoserScreen (void);
    inline void		PrintStory (void);
    inline void		WinnerScreen (void);
    inline void		DrawLevel (void);
    inline void		TitleKeys (key_t key);
    inline void		StoryKeys (key_t key);
    inline void		LevelKeys (key_t key);
    inline void		PutTile (PicIndex ti, int x, int y);
private:
    EGameState		_state;
    uint32_t		_storyPage;
    uint32_t		_level;
    SImage		_imgtiles;
    SImage		_imglogo;
    Level		_curLevel;
    vector<Level>	_levels;
};
