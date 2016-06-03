#include<bits/stdc++.h>

#include<dirent.h> //To get all files in the filesystem by recurssive traversal

#include<sys/stat.h> //To get file metadata info

using namespace std;

static int filecount=0;

struct FileData{
	string fileName;
	long int inode;
	string sum;
};// stucutre to store file metadata



//function to perform opration on duplicate files like merge or delete
void operation(){
	char input;
	string filepath,destpath,srcpath;
	cout<<"Do you want to Delete/Merge/No operation any of the above file [D/M/N]?  ";
	cin>>input;
	if(input=='D' || input=='d'){//To delete
		cout<<"\nEnter the file path you want to delete\n";
		cin>>filepath;
		//command to delete path
		system(("rm "+filepath).c_str());
	}
	else if(input=='M' || input=='m'){//To merge
		cout<<"Enter the file name in which you want to merge\n";
		cin>>destpath;
		cout<<"Enter the file name which you want to merge\n";
		cin>>srcpath;
		//command to merge path
		system(("merge "+destpath+" "+destpath+" "+srcpath).c_str());
		system(("rm "+srcpath).c_str());
	}
	else if(input=='N' || input=='n'){//No action

	}
	else{//Invalid choice
		cout<<"Enter valid choice\n";
		operation();
	}
}

//fuction to calculate md5sum of passed argument file and return string of md5sum
string fun (string filename){
	FILE *fpipe;
	stringstream md5sum;
	string str1="md5sum ";
	string str2=filename;
	string str3=" | awk '/ / {print $1}'"; //get only md5sum not the file name so awk programming to get first cloumn of o/p only
	stringstream ss;
	ss<<str1<<str2<<str3; //build the command to stringstream format

	string command=ss.str();

	char c = 0;
	//if command failed
	if (0 == (fpipe = (FILE*)popen(command.c_str(), "r")))
	{
		perror("popen() failed.");
		exit(1);
	}
	//if command success copy the result to stringstream
	while (fread(&c, sizeof c, 1, fpipe))
	{
		md5sum<<c;
	}

	pclose(fpipe);
	return  md5sum.str();//convert stringstream to string
}


//check is the file is file or directory and return boolean value
bool isDir(string dir){
	struct stat fileInfo;
	stat(dir.c_str(), &fileInfo);
	if (S_ISDIR(fileInfo.st_mode)){
		return true;
	}else{
		return false;
	}
}


//travese all the filesystem and stroe only files with their absolute path in the vector
void getdir(string dir, vector<string> &files, bool recursive){
	DIR *dp; //create the directory object
	struct dirent *entry; //create the entry structure
	dp=opendir(dir.c_str()); //open directory by converting the string to const char*
	if(dir.at(dir.length()-1)!='/'){
		dir=dir+"/";
	}
	if(dp!=NULL){ //if the directory isn't empty
		while( entry=readdir(dp) ){ //while there is something in the directory
			if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
				if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
					//files.push_back(string(entry->d_name)); //add entry of directory to the list of files
					getdir(dir + entry->d_name, files, true); //recurse
				} else{
					char real[2028];
					//get absolute path of file using builtin function realpath()
					realpath(string(dir+entry->d_name).c_str(),real);
					files.push_back((string)real);
					filecount++;
					cout<<"File Scanning Progress :["<<filecount<<"]\r";
					cout.flush();
				}
			}
		}
		(void) closedir(dp); //close directory
	}
	else{
		perror ("Couldn't open the directory.");
	}
}

//function for comparision of structure on the basis of md5sum
bool acompare(FileData lhs,FileData rhs){
	return lhs.sum < rhs.sum;
}

//main function
int main(int argc,char *argv[]){
	cout<<"Please enter the diretory path to list all duplicate files\n";
	//create the basic path to the directory
	string dir;
	cin>>dir;
	struct stat buff ;
	string fname;

	//create the list of strings
	vector<string> files = vector<string>();
	cout<<"\nBuilding the file list ...\n";
	//call the recursive directory traversal method
	getdir(dir,files,true);
	cout<<"\nFile list completed\n";
	cout<<"Total files scanned : "<<filecount<<endl<<endl;

	//data structure to store file info on the basis of key value pair where key is size of file
	map<long int,vector<FileData> > data;


	//container iterators
	map< long int,vector< FileData > > :: iterator itr;
	vector< FileData >  :: iterator vitr;
	vector< FileData >  :: iterator it;

	//list all files within the list.
	for(unsigned int i = 0;i<files.size();i++){
		stat(files[i].c_str(),&buff);
		FileData subObj;
		subObj.fileName = files[i];
		subObj.inode = buff.st_ino;
		data[buff.st_size].push_back(subObj);
	}

	cout<<"Duplicate file list is as follows\n\n";
	for( itr = data.begin(); itr!=data.end(); itr++) {
		if(itr->second.size()>1){

			for(vitr = itr->second.begin(); vitr != itr->second.end(); vitr++){
				(*vitr).sum=fun((*vitr).fileName);
			}//end of inner for

			//sort the structure on the basis of hashmap
			sort(itr->second.begin(),itr->second.end(),acompare);

			//two pointer method to iterate over structure
			it=itr->second.begin();
			vitr=itr->second.begin();
			it++;
			bool flag=true;
			bool entry=false;
			//iterat over all the structure of file having same file size
			while(it!=itr->second.end()){
				//if md5sum of two file is same then display file names
				if( !strcmp(  ((*vitr).sum).c_str(),((*it).sum).c_str()  ) ){
					if(flag){
						cout<<(*vitr).fileName<<endl<<(*it).fileName<<endl;
						flag=false;
					}
					else{
						cout<<(*it).fileName<<endl;
					}
					vitr++;
					it++;
					entry=true;
				}
				else{
					flag=true;
					cout<<endl;
					vitr=it;
					it++;
				}
			}//end of while

			if(entry){
				//call the operation function to perform merge or delete operation on dunplicate files
				operation();
				entry=false;
				cout<<endl;
			}

		}//end of inner for
	}//end of outer for
	return 0;
}





