#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <filesystem>
using namespace std;

//typedef void (*fptr)();

class cgame{
	public:
    	SDL_Window* window = NULL;
    	SDL_Renderer* renderer = NULL;
    	SDL_Texture* background = NULL;
    	vector<TTF_Font*> fonts;
    	vector<SDL_Texture*> images;

	    int screen_x = 0;
	    int screen_y = 0;
	    string name = "";
	    cgame(int x = 1280, int y = 860, string cap = "Screen"){
	    	screen_x = x;
	    	screen_y = y;
	    	name += cap;
    	}

	    bool init(){
		    bool success = true;
		    if(SDL_Init(SDL_INIT_VIDEO) < 0){
			    printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
			    success = false;
		    }
	    	else{
	        	window = SDL_CreateWindow(name.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_x, screen_y, SDL_WINDOW_SHOWN);
	        	if(window == NULL){
		        	printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
		        	success = false;
	        	}
	        	else{
	        		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
					if (renderer == NULL){
						printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
						success = false;
					}
					if(TTF_Init()==-1) {
    					printf("TTF_Init: %s\n", TTF_GetError());
					    success = false;
					}
	        	}
	      	}
	      	return success;
	    }

	    vector<SDL_Surface*> SurIms(vector<string> files){
	      	vector<SDL_Surface*> images;
	      	images.resize(files.size());
	        for (int i = 0; i < files.size(); i++){
	          images[i] = loadMedia(files[i]);
	        }
	        return images;
	    }

	    void loadFont(string path, int size){
	    	int i = fonts.size();
	    	fonts.resize(++i);
	    	fonts[--i] = TTF_OpenFont(path.c_str(), size);
	    }

	    SDL_Surface* loadMedia(string path){
	    	SDL_Surface* image;
	      	image = IMG_Load(path.c_str());
	      	if(image == NULL){
	        	printf("Unable to load image %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
	        	return NULL;
	      	}
	      	return image;
	    }

	    vector<SDL_Texture*> TexIms(vector<string> files){
	      	vector<SDL_Texture*> loadedimages;
	      	loadedimages.resize(files.size());
	        for (int i = 0; i < files.size(); i++){
	          loadedimages[i] = loadTexture(files[i]);
	        }
	        return loadedimages;
	    }

	    SDL_Texture* loadTexture(string path){
			SDL_Texture* newTexture = NULL;

			SDL_Surface* loadedSurface = IMG_Load(path.c_str());
			if(loadedSurface == NULL){
				printf("Unable to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());
			}
			else{
		        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
				if(newTexture == NULL){
					printf("Unable to create texture from %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
				}
				SDL_FreeSurface(loadedSurface);
			}
			return newTexture;
		}

	    void update(){
	    	SDL_RenderPresent(renderer);
	    }

	    void destroyTextures(vector<SDL_Texture*> loadedimages){
	      	for (int i = 0; i < images.size(); i++){
	        	SDL_DestroyTexture(images[i]);
	        	loadedimages[i] = NULL;
	      	}
	    }

	    void drawRect(bool solid, int start_x, int start_y, int end_x, int end_y, int red = 0xFF, int green = 0xFF, int blue = 0xFF){
	      	SDL_Rect fillRect = {start_x, start_y, end_x, end_y};
	      	SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
	      	if (solid){
	        	SDL_RenderFillRect(renderer, &fillRect);
	      	}
	      	else{
	        	SDL_RenderDrawRect(renderer, &fillRect);
	      	}
	    }

	    void drawLine(int start_x, int start_y, int end_x, int end_y, int red = 0xFF, int green = 0xFF, int blue = 0xFF){
	      	SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);        
	      	SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
	    }

	    void clear(){
	    	SDL_RenderClear(renderer);
	    	if(background == NULL){
	    		drawRect(1, 0,0, screen_x, screen_y, 0,0,0);
	    	}
	    	else{
	    		SDL_RenderCopy(renderer, background, NULL, NULL);
	    	}
	    }

	   	~cgame(){
	   		for (int i = 0; i < images.size(); i++){
	        	SDL_DestroyTexture(images[i]);
	        	images[i] = NULL;
	      	}

	      	for (int i = 0; i < fonts.size(); i++){
	        	TTF_CloseFont(fonts[i]);
	        	fonts[i] = NULL;
	      	}

	      	SDL_DestroyTexture(background);

	      	SDL_DestroyWindow(window);
	      	SDL_DestroyRenderer(renderer);
	      	window = NULL;

	      	TTF_Quit();
	      	IMG_Quit();
	      	SDL_Quit();
	    }
};

class Entity{
	public:
		int leftx = 0;
		int topy = 0;
		int velx = 0;
		int vely = 0;
		int accx = 0;
		int accy = 0;

		/*int top = 0;
		int left = 0;
		int bottom = 0;
		int right = 0;*/

		int width = 0;
		int height = 0; 

		SDL_Rect hbox;
		vector<int> colour = {0, 0, 0};

		cgame master;

		Entity(cgame* gameptr){
			master = *gameptr;
		}

		void setSizePos(int x, int y, int w, int h){
			leftx = x-w/2;
			topy = y-h/2;
			width = w;
			height = h;
			hbox.x = x;
			hbox.y = y;
			hbox.w = w;
			hbox.h = h;
		}

		void move(int tx, int ty, bool relative = 1){
			if (relative){
				leftx += tx;
				topy += ty;
			}
			else{
				leftx = tx;
				topy = ty;
			}
		}

		void draw(){
			hbox.x = leftx;
			hbox.y = topy;
			SDL_SetRenderDrawColor(master.renderer, colour[0], colour[1], colour[2], 0xFF);
	        SDL_RenderFillRect(master.renderer, &hbox);
			//master.drawRect(1, cenx - width/2, ceny - height/2, cenx + width/2, ceny + height/2, red, green, blue);
		}

		void update(){
			velx += accx;
			vely += accy;
			leftx += velx;
			topy += vely;
			if ((leftx+width)>master.screen_x){
				leftx = master.screen_x-width;
				velx = -velx;
			}
			if ((topy+height)>master.screen_y){
				topy = master.screen_y-height;
				vely = -vely;
			}
			if ((leftx)<0){
				leftx = 0;
				velx = -velx;
			}
			if ((topy)<0){
				topy = 0;
				vely = -vely;
			}
			draw();
		}
};

class Button{
	public:
		int left = 0;
		int top = 0;
		int width = 0;
		int height = 0;
		string cur = "";
		unordered_map<string, SDL_Texture*> ico;
		SDL_Rect ico_rect;


		vector<SDL_Rect> layerp;
		vector<vector<int>> layerc;

		vector<int> idle;
		vector<int> hover;
		vector<int> press;
		vector<int> release;

		vector<int>* stateptr = &idle;

		cgame* disptr;

		Button(cgame* ptr){
			disptr = ptr;
		}

		void setText(string t, SDL_Color colour = {0,0,0}, int fin = 0){
			if (ico.find(t)==ico.end()){
				SDL_Surface* surfaceMessage = TTF_RenderText_Solid((*disptr).fonts[fin], t.c_str(), colour);
				ico[t] = SDL_CreateTextureFromSurface((*disptr).renderer, surfaceMessage);
				SDL_FreeSurface(surfaceMessage);
			}
			cur = t;
		}

		void draw(){
			vector<int> itr = *stateptr;
			for(int i = 0; i< itr.size(); i++){
				SDL_Rect l = layerp[itr[i]];
				vector<int> c = layerc[itr[i]];
				SDL_SetRenderDrawColor((*disptr).renderer, c[0], c[1], c[2], 0xFF);
		        SDL_RenderFillRect((*disptr).renderer, &l);
	    	}
	    	if(!(cur=="")){
	    		SDL_RenderCopy((*disptr).renderer, ico[cur], NULL, &ico_rect);
	    	}
	    }

	    void addLayer(SDL_Rect box, vector<int> c){
	    	layerp.push_back(box);
	    	layerc.push_back(c);
	    }

	    int handle(SDL_Event* e, int x, int y){
	    	bool pressed = 0;
	    	SDL_Rect* hbp = NULL;
	    	if(layerp.size()){
	    		hbp = &layerp[0];
	    	}
	    	if(hbp == NULL){
	    		printf("Couldnt find hitbox of the button.\n");
	    	}
	    	else{
	    		SDL_Rect hb = *hbp;
				if(e -> type == SDL_MOUSEMOTION || e -> type == SDL_MOUSEBUTTONDOWN || e -> type == SDL_MOUSEBUTTONUP){
					bool inside = true;

					if(x < hb.x){
						inside = false;
					}
					else if(x > (hb.x + hb.w)){
						inside = false;
					}
					else if(y < hb.y){
						inside = false;
					}
					else if(y > (hb.y + hb.h)){
						inside = false;
					}
					if(!inside){
						stateptr = &idle;
					}
					else{
						switch(e -> type){
							case SDL_MOUSEMOTION:
								if(!(stateptr == &press)){
									stateptr = &hover;
								}
								break;
							case SDL_MOUSEBUTTONDOWN:
								stateptr = &press;
								pressed = 1;
								break;
							case SDL_MOUSEBUTTONUP:
								stateptr = &release;
								break;
						}
					}
				}
			}
			return pressed;
		}

	    ~Button(){
	    	for(auto& x: ico){
	    		SDL_DestroyTexture(x.second);
	    	}
	    	ico.clear();
	    }
};


vector<string> listdir(string path){
  	vector<string> files;
  	int i = 0;
    for (const auto & entry : filesystem::directory_iterator(path)){
      	files.resize(++i);
      	files[i-1] = entry.path();
    }
    return files;
}

int main(){
  	cgame Game;
  	if(!Game.init()){
    	printf("Failed to initialize. Quiting process.\n");
  	}
  	else{
    	string path = "c++/media/1.jpg";
    	Game.background = Game.loadTexture(path);
    	Game.loadFont("c++/media/Sans.ttf", 36);
    	int mX = 0;
    	int mY = 0;

    	float rfr = 1/30;
    	float t0 = time(0);

    	cgame* gameptr = &Game;
    	Entity player(gameptr);
    	player.colour = {255, 0, 0};
    	player.setSizePos(0, 0, 64, 64);

    	Button b1(gameptr);
    	b1.ico_rect = {325, 410, 350, 135};
    	b1.setText("omg", {255, 0, 0}, 0);
    	b1.addLayer({300, 400, 400, 200}, {50,50,50});
    	b1.addLayer({325, 425, 350, 150}, {150,150,150});
    	b1.addLayer({325, 425, 350, 150}, {200,200,200});
    	b1.addLayer({325, 425, 350, 150}, {100,100,100});

    	b1.idle = {0,1};
    	b1.hover = {0,2};
    	b1.press = {0,3};
    	b1.release = {0,2};

    	SDL_Event e;
    	bool quit = 0;
    	
    	while(!quit){
      		while(SDL_PollEvent(&e)){
      			SDL_GetMouseState(&mX, &mY);
      			if(b1.handle(&e, mX, mY)){
      				system("python3 .customapps/App.py");
      			}
        		switch(e.type){
          			case SDL_QUIT:
            			quit = 1;
            			break;
          			case SDL_KEYDOWN:
            			switch(e.key.keysym.sym){
            				case SDLK_ESCAPE:
			                case SDLK_q:
			                    quit = 1;
			                    break;
              				case SDLK_LEFT:
                				player.velx = -1;
                				break;
		            		case SDLK_RIGHT:
				                player.velx = 1;
				                break;
				            case SDLK_UP:
                				player.vely = -1;
                				break;
		            		case SDLK_DOWN:
				                player.vely = +1;
				                break;
				            default:
				            	break;
				    	}
		          	default:
		            	break;
		        }
		    }
	        Game.clear();
	        player.update();
	        b1.draw();
	       	Game.update();
	       	float tc = time(0);
	       	while ((tc - t0) < rfr){
	       		tc = time(0);
	       	}
      	}
    }
  	return 0;
}
