#include <bits/stdc++.h>

using namespace std;

struct Vector{
    double x,y,z;
};

typedef struct
{
  int index;
  double rad, speed;
}MOVE;

FILE* OpenFile(char *fileName)
{
  FILE *fp = fopen(fileName,"r");
  if(fp==NULL)
  {
    std::cerr << "ERROR while opening file" << std::endl;
    exit(-1);
  }
  return fp;
}

vector<Vector> GetBoundary(FILE *fp)
{
  vector<Vector> boundary;
  boundary.clear();
  for(int i=0;i<2;i++)
  {
    Vector pos;
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;
    fscanf(fp,"%lf %lf", &pos.x, &pos.y);
    boundary.push_back(pos);
  }
  return boundary;
}

MOVE GetSingleMovement(FILE *fp)
{
  int index = 0;
  double rad = 0.0, speed = 0.0;
  fscanf(fp, "%d %lf %lf", &index, &rad, &speed);
  return MOVE{index, rad, speed};
}

int main(){
    char movementFile[64] = {'\0'};
    strcpy(movementFile, "testcase_1");

    FILE *fp = OpenFile(movementFile);
    vector<Vector> edge;
    edge = GetBoundary(fp);

    int index = 0;
    double rad = 0.0, speed = 0.0;
    MOVE tmp;

    for(int i=0;i<edge.size();i++)
    {
        cout << edge[i].x << " " << edge[i].y << endl;
    }

    while(1)
    {
        tmp = GetSingleMovement(fp);
        cout << tmp.index << " " << tmp.rad << " " << tmp.speed << endl;
        if(tmp.index == 0)
            break;
    }


    return 0;
}
