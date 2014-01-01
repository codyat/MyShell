/* First Name: Cody 
 * Last Name: Troyer
 * email address: ctroy001@ucr.edu
 * 
 * I hereby certify that the contents of this file represent
 * my own original individual work. Nowhere herein is there 
 * code from any outside resources such as another individual,
 * a website, or publishings unless specifically designated as
 * permissible by the instructor or TA.
 */

#include <iostream>
#include <vector>
#include <sstream>
#include <errno.h>
#include <sys/wait.h>

using namespace std;

int go(string name, const char** list)
{
  if(execvp(name.c_str(), (char**)list) < 0)
    if(errno) return errno;
  return 0;
}

//function do it takes a vector of strings or operations from the user and runs them as a shell command
int do_it(const vector<string>&tok)
{
  if(tok.size() == 0) return 0;  
  string progname = tok[0];
  if(progname == "cd")                                               //checks if the operation is "cd"
  {
    if(tok.size() > 1)
    {
      string temp = tok[1];                                              //first string gets stored in progname
      if(temp[0] == '~') temp = (getenv("HOME") + temp.substr(1));
      chdir(temp.c_str());                                         //determines if cd has a path name after it
    }                                                                //change ~'s to home path name
    else chdir(getenv("HOME"));
    return 0;
  }
  pid_t kidpid;
  int status;
  const char** arglist = new const char*[tok.size() + 1];                                     //declares an argument list
  for(int i = 0; i != (int)tok.size(); i++)
  {
    if(tok[i] == "&" || tok[i] == ";") break;                        //if the next operation is a & or ; then exit and come back to them later
    else if(tok[i] == "<"){cerr<<"<"<<endl; freopen(tok[++i].c_str(), "r", stdin);}    //if the next operation is < then read in from a file
    else if(tok[i] == ">")
    {
      if((kidpid = fork()) == 0)
      {
	freopen(tok[++i].c_str(), "w", stdout); 
	go(progname, arglist);
        close(kidpid);
      }
      else
      {
        waitpid(kidpid, &status, 0);
	return status;
      }
    }
    else if(tok[i] == ">>")
    {
      if((kidpid = fork()) == 0)
      {
	freopen(tok[++i].c_str(), "a", stdout); 
	go(progname, arglist);
        close(kidpid);
      }
      else
      {
	waitpid(kidpid, &status, 0); 
	return status;
      }
    }                //if the next operation is >> then append to a file
    else if(tok[i] == "2>")
    {
      if((kidpid = fork()) == 0)
      {
	freopen(tok[++i].c_str(), "w", stderr);
	go(progname, arglist);
        close(kidpid);
      }
      else
      {
	waitpid(kidpid, &status, 0); 
	return status;
      }
    }                //if the next operation is 2> then write std errors to a file  
    else if(tok[i] == "|")
    {
      int mypipe[2];
      int &pipe_in = mypipe[0];
      int &pipe_out = mypipe[1];
      if(pipe(mypipe)) return errno;
/*      
if((kidpid = fork()) == 0)
      {
        close(pipe_out);
        while (read(pipe_in, &buf, 1) > 0)
          write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(pipe_in);
        execvp(tok[2].c_str(), arglist);
        return 0;
      }
      else 
      {            // Parent writes argv[1] to pipe
        close(pipe_in);          // Close unused read end 
        write(pipe_out, arglist[1], 1);
        close(pipe_out);          // Reader will see EOF
        waitpid(kidpid, &status, 0);
        execvp(tok[0].cstr()
        return 0;
      }
*/
      else if((kidpid = fork())) //you are the parent
      {
        dup2(pipe_out, 0);
        close(pipe_in);
        string temp = tok[2];
        waitpid(kidpid, &status, 0);
        go(temp, arglist);
      }
      else if((kidpid = fork()) == 0)//you are the kid
      {
        dup2(pipe_in, 1);
        close(pipe_out);
        go(progname, arglist);
      }
    }
    arglist[i] = tok[i].c_str();
  }
  if(progname[0] == '~') progname = (getenv("HOME") + progname.substr(1)); //change ~'s to home path name
  if((kidpid = fork()) < 0) return errno;
  else if(kidpid == 0)
  {
    go(progname, arglist);
    close(kidpid);
  }
  else
    if(tok[tok.size() - 1] != "&")
    {
      waitpid(kidpid, &status, 0);
      return status;
    }
  return 0;
}

int main()
{
  while(!cin.eof())                           //continue to prompt user for data until "exit" is entered
  {
    cout << "? ";
    string temp = "";
    getline(cin, temp);
    if(temp == "exit") return 0;
    stringstream ss(temp);
    while(ss)                                 //continues to store input from user into a vector of strings
    {
      vector<string> v;
      string s;
      while(ss >> s)
      {
        v.push_back(s);
	if(s == "&" || s == ";") break;
      }
      do_it(v);                  //performes the operation
    }
    if(errno) 
    {
      cerr << "myshell: " << strerror(errno) << endl;
      errno = 0;
    }
  }
  return 0;
}
