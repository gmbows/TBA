#pragma once

enum TBA_Texture { 
	TEX_GRASS,
}

struct Chunk {

	TBA_Texture texture;
	float x, y;
	float size;

}