#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
using namespace std;

const int width=1000;
const int height=1000;
float bullet_speed=8.0f;

int score=0;
int highest_score;

class Player{
    public:
        float x=width/2.0f;
        float y=height/2.0f;
        float speed=5.0f;
        float radius=30.0f;

        float rotation=0.0f;

        bool allow_left=1;
        bool allow_right=1;
        bool allow_up=1;
        bool allow_down=1;

        void Draw(){
            DrawCircle(x,y,radius,BLUE);
        }

        void Update(){
            if(y-radius<=0) allow_up=0;
            else allow_up=1;
            if(y+radius>=height) allow_down=0;
            else allow_down=1;

            if(x-radius<=0) allow_left=0;
            else allow_left=1;
            if(x+radius>=width) allow_right=0;
            else allow_right=1;

            //cout<<"left: "<<allow_left<<" right: "<<allow_right<<" up: "<<allow_up<<" down: "<<allow_down<<'\n';

            if(IsKeyDown(KEY_W)&&allow_up){
                y-=speed;
            }
            if(IsKeyDown(KEY_S)&&allow_down){
                y+=speed;
            }
            if(IsKeyDown(KEY_A)&&allow_left){
                x-=speed;
            }
            if(IsKeyDown(KEY_D)&&allow_right){
                x+=speed;
            }
        }
        bool Check_Enemy(float rad,float enemy_x,float enemy_y){
            Vector2 en={enemy_x,enemy_y};
            Vector2 play={x,y};
            return CheckCollisionCircles(en,rad,play,radius);
        }
        void Clean(){
            x=width/2.0f;
            y=height/2.0f;
        }
};

Player player;

struct bullet{
    float x;
    float y;
    float angle;
};

class Gun{
    public:
        float gun_width=45.0f;
        float gun_height=15.0f;
        float rotation=0.0f;
        float radians=0.0f;
        float bullet_radius=5.0f;

        Rectangle rec;
        Vector2 origin={-20.0f,gun_height/2.0f};

        vector<bullet> bullets;
        
        float last=0.0f;
        bool Timeout(float interval){
            float current=GetTime();
            if(current-last>=interval){
                last=current;
                return 1;
            }
            return 0;
        }

        void Angle(){
            Vector2 mouse=GetMousePosition();

            float dx=mouse.x-player.x;
            float dy=mouse.y-player.y;

            radians=atan2f(dy,dx);
            rotation=radians*(180.0f/PI);

            //cout<<rotation<<'\n';
        }

        void Shoot(){
            if(Timeout(0.6)){
                float barrel=gun_width-origin.x;
                float spawn_y=player.y+sinf(radians)*barrel;
                float spawn_x=player.x+cosf(radians)*barrel;

                bullet new_bullet={spawn_x,spawn_y,radians};

                bullets.push_back(new_bullet);

                //cout<<bullets.size()<<'\n';
            }
            
        }

        void Update_bullets(){
            if(!bullets.empty()){
                for(int i=bullets.size()-1;i>=0;i--){
                    bullets[i].x+=cosf(bullets[i].angle)*bullet_speed;
                    bullets[i].y+=sinf(bullets[i].angle)*bullet_speed;

                    if(bullets[i].x<=0||bullets[i].y<=0||
                       bullets[i].y>=height||bullets[i].x>=width) bullets.erase(bullets.begin()+i);
                }
            }
        }

        void Draw_bullets(){
            if(!bullets.empty()){
                for(int i=bullets.size()-1;i>=0;i--){
                    float bullet_x=bullets[i].x;
                    float bullet_y=bullets[i].y;
                    
                    DrawCircle(bullet_x,bullet_y,bullet_radius,BLACK);
                }
            }
        }

        void Draw(){
            Angle();

            rec={player.x,player.y,gun_width,gun_height};

            DrawRectanglePro(rec,origin,rotation,MAROON);
        }
        bool Check(float rad,float x,float y){
            if(!bullets.empty()){
                Vector2 en={x,y};
                for(int i=bullets.size()-1;i>=0;i--){
                    Vector2 bullet_t={bullets[i].x,bullets[i].y};
                    if(CheckCollisionCircles(en,rad,bullet_t,bullet_radius)){
                        bullets.erase(bullets.begin()+i);
                        return 1;
                    }
                }
            }
            return 0;
        }

};

Gun gun;

struct enemy_data{
    float x;
    float y;
    
};

class Enemy{
    public:
        float speed=1.0f;
        float radius=25.0f;

        vector<enemy_data> enemys={{200,100},{300,500}};

        float last=0.0f;
        bool timer(float interval){
            float current=GetTime();
            if(current-last>=interval){
                last=current;
                return 1;
            }
            return 0;
        }
        void New(){
            if(timer(2)){
                float new_x=GetRandomValue(0,width);
                float new_y=GetRandomValue(0,height);
                enemy_data new_enemy={new_x,new_y};
                enemys.push_back(new_enemy);
            }
            
        }
        void Update(){
            if(!enemys.empty()){
                for(int i=enemys.size()-1;i>=0;i--){
                    float x=enemys[i].x;
                    float y=enemys[i].y;

                    float dx=x-player.x;
                    float dy=y-player.y;

                    float radians=atan2f(dx,dy);

                    enemys[i].x-=speed*sinf(radians);
                    enemys[i].y-=speed*cosf(radians);
                }
            }
        }
        void Draw(){
            if(!enemys.empty()){
                for(int i=enemys.size()-1;i>=0;i--){
                    float x=enemys[i].x;
                    float y=enemys[i].y;

                    DrawCircle(x,y,radius,RED);
                }
            }
        }
        void Bullet_Check(){
            if(!enemys.empty()){
                for(int i=enemys.size()-1;i>=0;i--){
                    float x=enemys[i].x;
                    float y=enemys[i].y;

                    if(gun.Check(radius,x,y)){
                        enemys.erase(enemys.begin()+i);
                        score++;
                    }
                }
            }
        }
        bool Player_Check(){
            if(!enemys.empty()){
                for(int i=enemys.size()-1;i>=0;i--){
                    float x=enemys[i].x;
                    float y=enemys[i].y;

                    if(player.Check_Enemy(radius,x,y)){
                        return 1;
                    }
                }
            }
            return 0;
        }
        void Clear(){
            enemys.clear();
        }

};

Enemy enemy;

void Load_Score(){
    ifstream fin ("data.txt");
    if(fin.is_open()){
        fin>>highest_score;
        fin.close();
    }
    else{
        highest_score=0;
    }
}

void Save_Score(int new_score){
    if(new_score>highest_score){
        highest_score=new_score;
        ofstream fout ("data.txt",ios::trunc);
        if(fout.is_open()){
            fout<<highest_score;
            fout.close();
        }
    }
}

float last=0.0f;
bool Clock(float interval){
    float current=GetTime();
    if(current-last>=interval){
        last=current;
        return 1;
    }
    return 0;
}

int main(){
    InitWindow(width,height,"Shooter");
    SetTargetFPS(60);
    int state=0;

    Load_Score();

    Rectangle start={width/2-150,height/2-50,300,100};
    const string text_start="Press to start";
    const int size=MeasureText(text_start.c_str(),30);
    
    while(!WindowShouldClose()){
        Vector2 mouse=GetMousePosition();

        switch(state){
            case 0:{
                break;
            }
            case 1:{
                player.Update();
                enemy.Update();
                gun.Update_bullets();
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    gun.Shoot();
                }
                enemy.Bullet_Check();
                if(enemy.Player_Check()){
                    enemy.Clear();
                    Save_Score(score);
                    player.Clean();
                    score=0;
                    state=2;
                }
                enemy.New();
                break;
            }
            case 2:{
                break;
            }
        }
        BeginDrawing();
        switch(state){
            case 0:{
                ClearBackground(WHITE);
                DrawRectangleRec(start,(CheckCollisionPointRec(mouse,start)) ? MAROON : RED);
                DrawText(text_start.c_str(),(width-size)/2,height/2,30,WHITE);

                const string high=TextFormat("Highest score: %d",highest_score);
                const int size_high=MeasureText(high.c_str(),50);

                DrawText(high.c_str(),(width-size_high)/2,100,50,BLACK);
                if(CheckCollisionPointRec(mouse,start)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    state=1;
                }
                break;
            }
            case 1:{
                ClearBackground(LIGHTGRAY);
                player.Draw();
                enemy.Draw();
                gun.Draw();
                gun.Draw_bullets();
                DrawText(TextFormat("Score: %d",score),(width-MeasureText(TextFormat("Score: %d",score),30))/2,20,30,BLACK);
                DrawFPS(10,10);
                break;
            }
            case 2:{
                DrawText("Game Over",(width-MeasureText("Game Over",50))/2,height/2,50,BLACK);
                if(Clock(2)){
                    state=0;
                }
                break;
            }
        }
        EndDrawing();
        //cerr<<state<<'\n';
    }
    CloseWindow();
    return 0;
}
