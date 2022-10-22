//*********************** All Header files*********************
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<time.h>
#include<string.h>
#include<termios.h>
#include<iostream>
#include<pwd.h>
#include<grp.h>
#include<set>
#include<bits/stdc++.h>
#include <sys/ioctl.h>
#include<sys/wait.h>
#include<math.h>
#define HEADERS_H
using namespace std;
#define MAX 15

//******************** Define Global Variables****************
char present_folder[PATH_MAX];
stack<string> stack1,stack2;
struct winsize window_size;
int no_rows;
int no_col;
string current_dir;
vector <string> list_down_all_directory(const char*,int emp);
int is_Dir(string sys);
void print_directory();
void Command_Mode(vector<string> list,int w);
void copy_file_cmd(vector<string> cmd_mod);
void tap_delete_irectory(string source);
void clear_screen3();
struct termios prev_set;
struct termios new_set;
vector<string> result;
int Cmd_m;
int num;
vector<string> my_final2;
string state="";
string eat="";
int mode=0;
int start_i=0;
int end_i=start_i+MAX;
int blink=1;
vector<string> my_final;
int counter=0;

//*******************cursor position***************
void cursor_pointer(int x,int y){
    int val=27;
    printf("%c[%d;%dH",val,x,y);

    //  _setcursortype(_SOLIDCURSOR);
    
}

//***************set cursor in command mode************
void Set_command(int n, int *y)
{
   cursor_pointer(18,*y);
   clear_screen3();
   printf("command mode:");
   cout<<endl;
   cout<<endl;
   printf("--->"); 
    *y=5;
}

//********************clear screen******************
void clear_screen(){
    printf("\33c");
}
void clear_screen1(){
    printf("\033[1A");
}
void clear_screen2(){
    printf("\033[1B");
}
void clear_screen3(){
    printf("\e[0J");
}
void clear_screen4(){
    printf("\x1b[0K");
}
//*********************scroll Up********************
void scrollUp(){
    if(blink>1){
        blink--;
        cursor_pointer(blink,0);
        return ;
    }
    if(start_i!=0){
        start_i--;
        end_i--;
        print_directory();
        cursor_pointer(blink,0);
    }
}
//************************scroll Down******************
void scrollDown(){
    if(blink<my_final.size() && blink<MAX){
        blink++;
        cursor_pointer(blink, 0);
        return;
    }

    if (end_i !=my_final.size()) 
    {
        start_i++;
        end_i++;
        print_directory();
        cursor_pointer(blink, 0);
    } 
}

//************** switching between canonical and non-canonical mode**************
int Getch()
{
 int key;
 tcgetattr(0, &prev_set); 
 new_set=prev_set; 
 new_set.c_lflag &= ~ICANON;
 new_set.c_lflag &= ~ECHO; 
 tcsetattr(0, TCSANOW, &new_set);

  key=cin.get();   //Taking input from keyboard
                   
 if(tcsetattr(0, TCSANOW, &prev_set)) 
    {
        fprintf(stderr,"failed to set new attributes");
        exit(1);
    }
     return key; 
}

//*****************split string***************************
vector<string> split_string(string str1, string str2)
{
    string toss;
    int loc;
    vector<int>tej;
    vector<string>res;
    loc= str1.find(str2);
    while (loc!= string::npos){
        toss = str1.substr(0, loc);
        res.push_back(toss);
        int pp=loc + str2.length();
        str1.erase(0,pp);
        loc = str1.find(str2);
    }
    res.push_back(str1);
    return res;
}

//***********searching files and directories***************** 
void search_cmd(string path, vector<string>blink)
{
    int ss=blink.size();
    if(ss<2 || ss>2){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else {
        DIR *dir;
        struct dirent *ds;
        string src =path+"/"+blink[1];
        dir=opendir(path.c_str());
        if(dir==NULL){
            printf("error occurs:- %s",path.c_str());
        }
        while((ds=readdir(dir))!=NULL)
        {       
            if(!strcmp(ds->d_name,".")){
                continue;
            }
            if(!strcmp(ds->d_name,"..")){
                continue;
            }
            string destination=path+"/"+ds->d_name;
            if(!strcmp(src.c_str(),destination.c_str())){
                result.push_back(destination);
            }
        }    
    }
}

//***********************Finding absolute path*********************
string Absolute_path_find(string path78)
{
    string absolute_pa="";
    if (path78[0] == '~'){ 
        absolute_pa=string(present_folder)+path78.substr(1,path78.length());
    }

    else if (path78[0] == '/'){ 
        absolute_pa=string(present_folder)+path78;
    }

    else if (path78[0] == '.' && path78[1] == '/'){ 
        absolute_pa= current_dir + path78.substr(1, path78.length());
    }

    else{ 
        absolute_pa= current_dir + "/" + path78;
    }

    return absolute_pa;
}

//*******************delete file***********************
void tap_delete_irectory(string source)
{
    DIR *dirty;
    if(opendir(source.c_str())==NULL){
        printf("error occurs: %s",source.c_str());
    }
    struct dirent *sdd;  
    string str1;
    string str2;  
    while((sdd=readdir(dirty))!=NULL)
    {
        if(!strcmp(sdd->d_name,"..")){
            continue;
        }
        if(!strcmp(sdd->d_name,".")){
            continue;
        }
        str2=sdd->d_name;
        str1=source+"/"+str2; 
        if(!is_Dir(str1))
        {
            if(unlink(str1.c_str())==-1)
            { 
                printf("error occurs: %s ",source.c_str());
                return;
            }
        }
        else{
            tap_delete_irectory(str1);
            if(rmdir(str1.c_str())==-1)
                printf("error occurs: %s", source.c_str());
        }  
    }
}

//*****************move files*******************
void move_file_cmd(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<3 || ss>3){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else{
        copy_file_cmd(cmd_mod);
            int i=1;
            while(i<ss-1){
            string source;
            source=Absolute_path_find(cmd_mod[i]);
            if(is_Dir(source))
            {   
                tap_delete_irectory(source);
                if(rmdir(source.c_str())==-1){
                    eat="Invalid Command Try Again!!";
                    cout<<eat;
                }
                else{ 
                    list_down_all_directory(current_dir.c_str(),no_rows-3);
                }
            }
            else if(unlink(source.c_str())==-1){
                eat="Invalid Command Try Again!!";
                cout<<eat;
            }
            else{ 
               list_down_all_directory(current_dir.c_str(),no_rows-3); 
            }
             i++;  
            }
        }  
}

//****************find permission type************
mode_t getting_per(string source)
{
    struct stat nodeid;
    stat(source.c_str(), &nodeid);
    mode_t m=0;
    m = m | ((nodeid.st_mode & S_IROTH)?0004:0);
    m = m | ((nodeid.st_mode & S_IWOTH)?0002:0);
    m = m | ((nodeid.st_mode & S_IXOTH)?0001:0);

    m = m | ((nodeid.st_mode & S_IRUSR)?0400:0);
    m = m | ((nodeid.st_mode & S_IWUSR)?0200:0);
    m = m | ((nodeid.st_mode & S_IXUSR)?0100:0);

    m = m | ((nodeid.st_mode & S_IRGRP)?0040:0);
    m = m | ((nodeid.st_mode & S_IWGRP)?0020:0);
    m = m | ((nodeid.st_mode & S_IXGRP)?0010:0);
    return m;
}

//**********************copy part3*********************
void copy_file_dir(string source, string destiny)
{
    int sdd;
    int hdd;
    char cht;
    if((sdd=open(source.c_str(),O_RDONLY))==-1){
        eat="Invalid Command Try Again!!";
        cout<<eat;
        return;
    }
    mode_t m = getting_per(source);
    if((hdd=open(destiny.c_str(),O_CREAT|O_WRONLY,m))==-1){
        printf("file already exists");
        return;
    }
    while(read(sdd,&cht,1)){
        write(hdd,&cht,1);
    }
    close(sdd);
    close(hdd);
}

//*****************copy part2*****************
void  copy_directory_file(string source, string destiny)
{
    string str1;
    string str2;
    DIR *dirty;
    struct dirent* sdd;
    dirty=opendir(source.c_str());
    if(dirty==NULL)
    {
        eat="Invalid Command Try Again!!";
        cout<<eat;
        exit(1);
    }
    else{
    while((sdd=readdir(dirty))!=NULL){
        if(!strcmp(sdd->d_name,".")){
            continue;
        }
        if(!strcmp(sdd->d_name,"..")){
            continue;
        }
        str1=source+"/"+sdd->d_name; 
        str2=sdd->d_name;
        if(!is_Dir(str1)){
            copy_file_dir(str1,str2);
        }
        else{
            mode_t m;
            m = getting_per(str1);
            if(mkdir(str2.c_str(),m)==-1){
                eat="Invalid Command Try Again!!";
                cout<<eat;
                return;
            }
            else 
                copy_directory_file(str1,str2);
        }
        
    }
    }
}

//********checking is it directory or not********
int is_Dir(string sys)
{
    struct stat ghj;
    if(stat(sys.c_str(), &ghj) != 0) {
        return 0;
    }
    if(S_ISDIR(ghj.st_mode)){
        return 1;
    }
    else{
        return 0;
    }
}

//***************copying file*****************
void copy_file_cmd(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<3 || ss>3){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else {
        string desttnation_folder=Absolute_path_find(cmd_mod[ss-1]);
        if(!(is_Dir(desttnation_folder)))
         {
            eat="Invalid Command Try Again!!";
            cout<<eat;
            return ;
         }
        int i=1;
        while(i<ss-1){
            string source=Absolute_path_find(cmd_mod[i]);
            vector<string> names=split_string(cmd_mod[i],"/");
            string dest=desttnation_folder+"/"+names[names.size()-1];
            if(is_Dir(source)){
                mode_t m;
                m = getting_per(source);
                if(mkdir(dest.c_str(),m)==-1){
                    eat="Invalid Command Try Again!!";
                    cout<<eat;
                    return;
                }
                copy_directory_file(source,dest);  
            }
            else 
                copy_file_dir(source,dest);

            i++;           
        }
    }
}

//**********************delete file******************
void delete_file_cmd(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<2 || ss>2){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else{
        string src=Absolute_path_find(cmd_mod[1]);
        if(unlink(src.c_str())==-1){
            eat="Invalid Command Try Again!!";
            cout<<eat;
        }
        else{ 
            list_down_all_directory(current_dir.c_str(),no_rows-3);
        }
    }
}

//**********************creating folder************** 
void create_dir_cmd(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<3 || ss>3){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else{
        string sfg=cmd_mod[2];
        string dest_folder_loc=Absolute_path_find(sfg);
        string create_folder_name=cmd_mod[1];
        string dest_file_location=dest_folder_loc+"/"+cmd_mod[1];
        if(mkdir(dest_file_location.c_str(),0755)==-1){
            eat="Invalid Command Try Again!!";
            cout<<eat;
        }
        else{ 
            list_down_all_directory(current_dir.c_str(),no_rows-3);
        }
    }
}

//*********************creating file**************************
void create_File_cmd(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<3 || ss>3){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else{
        string cdg=cmd_mod[2];
        string dest_directory=Absolute_path_find(cdg);
        string create_file_name=cmd_mod[1];
        string dest_file_location=dest_directory+"/"+cmd_mod[1];
        if(creat(dest_file_location.c_str(),0600)==-1){
            eat="Invalid Command Try Again!!";
            cout<<eat;
        }
        else{ 
            list_down_all_directory(current_dir.c_str(),no_rows-3);
        }
    }  
}

//*************************Rename file**************************
void rename_command(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<3 || ss>3){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }

    else {
        string src=Absolute_path_find(cmd_mod[1]);
        string dest=Absolute_path_find(cmd_mod[2]);
        vector<string>gd=split_string(src,"/");
        vector<string>ga=split_string(dest,"/");
        int p=gd.size();
        int q=ga.size();
        gd[p-1]=ga[q-1];
        string tan=gd[0];
        int i=1;
        while(i<gd.size()){
            tan=tan+"/"+gd[i++];
        }
        if(rename(src.c_str(),tan.c_str())==-1){
            eat="Invalid Command Try Again!!";
            cout<<eat;
        }
        else{
            list_down_all_directory(current_dir.c_str(),no_rows-3);
        }
    }   
}

//*********************Goto command**********************
void goto_command(vector<string> cmd_mod)
{
    int ss=cmd_mod.size();
    if(ss<2 || ss>2){
        eat="Invalid Command Try Again!!";
        cout<<eat;
    }
    else{
        string src=Absolute_path_find(cmd_mod[1]);
        vector<string> path;
        path=split_string(src,"/");
        int spc=path.size();
        if(path[spc-1]=="."){
            return;
        }
        stack1.push(current_dir);
        if(path[spc-1]==".."){
            string dsap=src.substr(0,src.length()-1);
            vector<string>path23=split_string(dsap,"/");
            string dir=path23[0];
            int i=1;
            while(i<path23.size()-2){
                dir=dir+"/"+path23[i];
                i++;
            }
            list_down_all_directory(dir.c_str(),no_rows-3);    
        }
        else
            list_down_all_directory(src.c_str(),no_rows-3);
    }
}

//**************************print directories and files************************
void print_directory()
{
    // if(start_index<last_index){
    clear_screen();
    cursor_pointer(0,0);
    struct stat jss;
    double size;
    char timing[100];
    char temp[40];
    // int dd=min()
    for(int i=start_i;i<end_i && i<=my_final.size();i++){
        string str=my_final[i];
    string permission="";

    struct tm local_time;

        stat((str).c_str(),&jss); 
        permission+=(S_ISDIR(jss.st_mode) ? "d" : "-");
        permission+=((jss.st_mode & S_IRUSR) ? "r" : "-");
	    permission+=((jss.st_mode & S_IWUSR) ? "w" : "-");
	    permission+=((jss.st_mode & S_IXUSR) ? "x" : "-");
	    permission+=((jss.st_mode & S_IRGRP) ? "r" : "-");
	    permission+=((jss.st_mode & S_IWGRP) ? "w" : "-");
	    permission+=((jss.st_mode & S_IXGRP) ? "x" : "-");
        permission+=((jss.st_mode & S_IROTH) ? "r" : "-");
	    permission+=((jss.st_mode & S_IWOTH) ? "w" : "-");
	    permission+=((jss.st_mode & S_IXOTH) ? "x" : "-");
        
        
        printf("\033[7;1;36m");
        cout<<"-->  ";
        printf(" %s  |",permission.c_str()); // print permission section
        size=jss.st_size;
        int MAX1=1024;
        if(size>100)
        {
          size=size/(MAX1);
          if(size>100)
           {
                size=size/(MAX1);
                printf("%10.2fM   |",size);
            }
    
           else{ 
                printf("%10.2fK   |",size); // print size section
           }
        }    
        else{
            printf("%10.2fB   |",size);
        } 

        struct passwd * pws;
        struct group * gp;
        uid_t user_id;
        gid_t group_id;
            user_id=geteuid();
            pws=getpwuid(user_id);
            if(pws){
                printf("  %s  |", pws-> pw_name); //print user id
            }

            group_id=getegid();
            gp=getgrgid(group_id);
            if(gp){
                printf("  %s  | ",gp->gr_name); // print group id
            }

        strcpy(timing,ctime(&jss.st_mtime)); 
        time_t gb_time;
        gb_time = jss.st_mtime;
        localtime_r(&gb_time, &local_time);
        strftime(timing, sizeof(timing), "%b %d %H:%M", &local_time);
        
        printf(" %10s   |",timing); //print last updated time

        vector<string> name;
        name=split_string(str, "/");
        int os=name.size();
        os=os-1;
        cout<<"   "<<name[os]<<"  "<<endl; //print file and directory names
    }
    cout<<endl;
    cout<<endl;
    cout<<"*******************************NORMAL MODE*****************************"<<endl;
    cout<<endl;
    cout<<"<-----  Press : To Change Into Command Mode  ----->";

}

// ******************cursor movement functionality**********************//
void cursor_movement(vector<string> list_dir,int emp)
{
    string homedir =present_folder;
    if(emp<list_dir.size()){
        num=emp;
    }
    else{
        num=list_dir.size();
    }

    struct stat jss;

    cursor_pointer(0,0); //initially Set cursor
    int input=-1;
    int pos=0;
    int k=-1;
    int l=-1;
    int ri=1;
// for upward press "up arrow" or "A"
// for downward press "down arrow" or "B"
// for open any file or directory press "enter"
// for directly jump to home press "h"
// for goto one level up press "backspace"
// for closing the application press "q"
// for change to Command mode press ":"

    while(ri)
    {
        input=0;
        input=Getch();
        if(input==65 || input==66){
            if(input==65 && pos>0){  // for Up Arrow
                pos=pos-1;
                scrollUp();
            }
            if(input==66 && pos<num-1){ //for Down Arrow
            pos=pos+1;
                scrollDown();
            }   
        }

        if(input==68){              //backward Arrow
            int ss=stack2.empty();
            if(ss==0)
            {
                string ug=stack2.top();
                stack2.pop();
                stack1.push(current_dir);
                list_down_all_directory(ug.c_str(),emp);
            }
        }
        if(input==67){            //forward Arrow
            int ss=stack1.empty();
            if(ss==0){
                string ug=stack1.top();
                stack1.pop();
                stack2.push(current_dir);
                list_down_all_directory(ug.c_str(),emp);
            }
        }
        if(input==10){    //for open file or directory
            if(k>0){
                pos=pos+k;
            }
            if(pos!=0){
                stat((list_dir[pos]).c_str(),&jss);
                 if(S_ISDIR(jss.st_mode))
                    {    
                        int my_pos=pos;
                        if(my_pos!=1 && my_pos!=0){
                            stack2.push(current_dir);
                            list_down_all_directory((list_dir[my_pos]).c_str(),emp);
                        }                             
                    }
                else {
                    const char *file_name;
                    file_name=list_dir[pos].c_str();
                    if (fork() == 0) {
		                execl("/usr/bin/xdg-open", "xdg-open", file_name, (char *)0);
		                    ri=0;
	                }
                }    
            }
        }
        if(input==127 || input==104){     
            while(!stack1.empty()){
                stack1.pop();
            }

            stack2.push(current_dir);
            int d=1;
            if(d==1){
                if(input==127){             //Go one level up
                    list_down_all_directory((list_dir[1]).c_str(),emp);
                }
                if(input==104){      //jump directly to home
                    list_down_all_directory(homedir.c_str(),emp); 
                }                
            }            
        }       

        if(input==113){  //for closing the application
            int gs=num; 
            gs+=2;
            cursor_pointer(gs,0);
            int gpl=27;
            printf("%c[%dK",gpl,2);
            cursor_pointer(22,0);
            printf("%c[%dK",gpl,2);
            printf("\t\t\t\t");
            cout<<endl;
            cout<<"<-------------------------GOOD BYE------------------------->"<<endl;
            cout<<endl;
            ri=0;
            exit(1);

        }  
        if(input==58){   //switch into command mode;
            mode=1;
            Command_Mode(list_dir,emp);
        }      
    }
}

//************************command mode functinality****************************
void Command_Mode(vector<string>command_list,int w)
{
    int num2;
    int sz=15;
    string canonical="";
    vector<string> command;
    if(w<sz){
        num2=w;
    }
    else{
        num2=sz;
    }
    num2+=2; 
    Cmd_m=num2;
    int y=0;
    Set_command(num2,&y);
    cursor_pointer(num2+3,y);
    char input;
    char a,b;
    Cmd_m=1;
    int ri=1;
   
    while(ri)
    {  
        input=0;
        input=Getch();
        if(input==27){     //back to normal mode
            mode=0;
            Cmd_m=0;
            int pk=num2;
            cursor_pointer(pk+2,0);
            clear_screen3();
            cursor_movement(command_list,w);
        }
        else if(input==10){  
                y=0;
                Set_command(num2,&y);
                command=split_string(canonical," ");
                    if(command[0].compare("search")==0){ // searching file or directory
                        state=command[0];
                        result.clear();
                        search_cmd(current_dir,command);
                        if(result.size()){
                            printf("True");
                        }
                        else {
                            printf("False");
                        }
                    }   
                    canonical="";
                    if(command[0].compare("quit")==0){ //close the application
                        cursor_pointer(num2,0);
                        int gpl=27;
                        printf("%c[%dK",gpl,2);
                        ri=0;
                        cout<<endl;
                        // printf("\t\t\t\t");
                        cout<<"<-------------------------GOOD BYE------------------------->"<<endl;
                        cout<<endl;
                        exit(1);
                    }
                    if(command[0].compare("goto")==0){  //goto command
                        goto_command(command);
                    }
                    if(command[0].compare("rename")==0){ //rename the file or directory
                        rename_command(command);
                    }
                    if(command[0].compare("create_file")==0){  //create a file
                        create_File_cmd(command);
                    }   
                    if(command[0].compare("create_dir")==0){  //create a folder
                         create_dir_cmd(command);
                    }
                    if(command[0].compare("delete_file")==0){  //delete file
                        delete_file_cmd(command);
                    }
                    if(command[0].compare("copy")==0){   //copy file
                        copy_file_cmd(command);
                    }
                    if(command[0].compare("move")==0){  //move file
                        move_file_cmd(command);
                    }
                    
            }
        else if(input==127){  //removing chararter while taking input
                    if(y>5){
                        int puff=y;
                        puff=puff-1;
                        y--;
                        cursor_pointer(num2+3,puff);
                        clear_screen4();
                        canonical=canonical.substr(0,canonical.length()-1);
                    }
            }

        else{                  //taking command input from user;
        if(state=="search"){
            y=0;
            Set_command(num2,&y);
            cursor_pointer(num2+3,y);
            state="";
        } 
        if(eat=="Invalid Command Try Again!!")  {
            y=0;
            Set_command(num2,&y);
            cursor_pointer(num2+3,y);
            eat="";
        }        
            cout<<input; 
            int puff=y;
            puff=puff+1;
            y++;
            cursor_pointer(num2+3,puff);
            canonical=canonical+input;
        }
   }
}

//******************* list_down_all_directory****************
vector<string> list_down_all_directory(const char *directory_name,int emp)
{
    int enem=emp;
    DIR *file;
    struct dirent *temp;
    // vector<string> my_final;
    my_final.clear();
    vector<string> rep;
    rep.push_back(" ");
  
    file=opendir(directory_name);  //open directory
    if(file==NULL)
    {
        eat="Invalid Command Try Again!!";
        cout<<eat;
        return rep;
    }

    //Iterate through each files and directories and stored in a vector;
    temp=readdir(file);
    while(temp!=NULL)
       { 
         string str1=temp->d_name;
         string str2=directory_name;
         str1=str2+"/"+str1;
         my_final.push_back(str1);
         temp=readdir(file);
       }

    closedir(file);    
    
    sort(my_final.begin(),my_final.end()); //sort the files

    int until=my_final[0].size();
    until=until-2;
    current_dir=my_final[0].substr(0,until);
    
   clear_screen(); //before printing the content clear the screen;

    emp=enem;
    int n;
    int z=my_final.size();
    start_i=0;
    if(z>=MAX){
        end_i=MAX;
    }
    else{
        end_i=z;
    }
        print_directory();  //print directoies
    if(mode==0){
        cursor_movement(my_final,emp); //fix the cursor;
    }
    else{
        Command_Mode(my_final,emp);
    }
    return my_final;
}

//***************************main function************************
int main()
{
    vector<string> my_final;
    int size_of_folder=sizeof(present_folder);
    getcwd(present_folder,size_of_folder); //fetch path of current working directory
  
    ioctl(STDIN_FILENO, TIOCGWINSZ, &window_size);
    no_rows=window_size.ws_row;
    no_col=window_size.ws_col;
    int pras=no_rows;

    //call function to list all directories;
    my_final=list_down_all_directory(present_folder,pras); 
    for(int i=0;i<my_final2.size();i++){
        my_final2.push_back("");
    }
  return 0;
}