#include "raylib.h"

class TextBox
{
private:
	Rectangle render;
	std::string text;
	int maxLength;
	int fontSize;
	int i;
	bool focus;
public:
	Color backCol;
	Color textCol;
	TextBox(Rectangle r, const char * t, int max, int size = 40)
	{
		render = r;
		text.assign(t);
		if (text.size() > max)
		{
			text = text.substr(0, max);
		}
		maxLength = max;
		fontSize = size;
		i = text.size();
	}
	
	Vector2 pos()
	{
		
		return (Vector2){render.x, render.y};
	}
	
	void move(int x, int y)
	{
		render.x = x;
		render.y = y;
	}
	
	void move(Vector2 coord)
	{
		render.x = coord.x;
		render.y = coord.y;
	}
	
	Vector2 size()
	{
		
		return (Vector2){render.width, render.height};
	}
	
	void resize(int w, int h)
	{
		render.width = w;
		render.height = h;
	}
	
	void resize(Vector2 dimensions)
	{
		render.width = dimensions.x;
		render.height = dimensions.y;
	}
	
	int max()
	{
		
		return maxLength;
	}
	
	std::string constrain(int length)
	{
		maxLength= length;
		if (text.size() > length)
		{
			text = text.substr(0, length);
		}
		i = text.size();
		
		return text;
	}
	
	bool isFocus()
	{
		focus = CheckCollisionPointRec(GetMousePosition(), render);
		
		return focus;
	}
	
	int index()
	{
		
		return i;
	}
	
	std::string contents()
	{
		
		return text;
	}
	
	void draw()
	{
		DrawRectangleRec(render, LIGHTGRAY);
		DrawText(text.c_str(), render.x + 5, render.y + 8, 40, MAROON);
	}
	
	void collect()
	{
		if ( isFocus() )
		{
			SetMouseCursor(MOUSE_CURSOR_IBEAM);
			int key = GetCharPressed();
			// Check if more characters have been pressed on the same frame
			while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ( (key >= 32) && (key <= 125) && (i < maxLength) )
                {
					std::cout << text << std::endl;
					text.push_back((char)key);
					i++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }
			if ( IsKeyPressed(KEY_BACKSPACE) )
			{
				i--;
				if ( i < 0 )
				{
					i = 0;
				}
				else
				{
					text.pop_back();
				}
				std::cout << text << std::endl;
			}
		}
	}
};

class TextButton
{
private:
	Rectangle render;
	bool focus;
	bool hover;
	int fontSize;
public:
	std::string text;
	Color backCol;
	Color textCol;
	TextButton(Rectangle r, const char * t, int size = 40, Color background = LIGHTGRAY, Color col = MAROON)
	{
		render = r;
		text.assign(t);
		fontSize = size;
		focus = false;
		backCol = background;
		textCol = col;
	}
	
	Vector2 pos()
	{
		
		return (Vector2){render.x, render.y};
	}
	
	void move(int x, int y)
	{
		render.x = x;
		render.y = y;
	}
	
	void move(Vector2 coord)
	{
		render.x = coord.x;
		render.y = coord.y;
	}
	
	Vector2 size()
	{
		
		return (Vector2){render.width, render.height};
	}
	
	void resize(int w, int h)
	{
		render.width = w;
		render.height = h;
	}
	
	void resize(Vector2 dimensions)
	{
		render.width = dimensions.x;
		render.height = dimensions.y;
	}
	
	bool isFocus()
	{
		hover = CheckCollisionPointRec(GetMousePosition(), render);
		focus = hover && IsMouseButtonReleased(0);
		
		return focus;
	}
	
	bool isHover()
	{
		hover = CheckCollisionPointRec(GetMousePosition(), render);
		
		return CheckCollisionPointRec(GetMousePosition(), render);
	}
	
	std::string contents()
	{
		
		return text;
	}
	
	void draw()
	{
		Color col = backCol;
		if (hover)
		{
			col = Color{ backCol.r - 10, backCol.g - 10, backCol.b - 10, 255 };
		}
		DrawRectangleRec(render, col);
		int centerX = render.x + render.width / 2 - MeasureText(text.c_str(), fontSize / 2);
		int centerY = render.y + render.height / 2 - fontSize / 2;
		DrawText(text.c_str(), centerX, centerY, fontSize, textCol);
	}
	
	void collect()
	{
		if ( isHover() )
		{
			// TODO: change colour
			if ( IsMouseButtonReleased(0) )
			{
				// TODO: indent
			}
		}
	}
};