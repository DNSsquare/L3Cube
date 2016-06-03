#include 	<bits/stdc++.h>
#define 	DRIVE_NAME          "myDrive.dns2"
#define 	CMD_FORMAT 	        "format"
#define 	CMD_LS 		        "ls"
#define 	CMD_CREATE          "create"
#define 	CMD_EXIT	        "exit"
#define 	CMD_ECHO	        "echo"
#define 	CMD_COPY	        "copy"
#define 	CMD_DELETE	        "delete"
#define		CMD_SAVE_DRIVE	    "save"
#define		CMD_RESTORE_DRIVE   "restore"
#define 	HARD_DISK_SIZE		10 * 1024 * 1024
#define		MAX_METADATA_SIZE 	1  * 1024 * 1024
#define 	MAX_DRIVEDATA_SIZE	9  * 1024 * 1024
using namespace std;

struct inode_info { //SAVES INODE COUNT
    int inodes;
};

class file_metadata { //SAVE METADATA OF FILE
public:
    int id;
    int start_block;
    int length;
    char filename[20];

    friend bool operator<(const file_metadata f1, const file_metadata f2);
};

vector <file_metadata> file_table;	//FILE TABLE
char mydrive[MAX_DRIVEDATA_SIZE];	//DATA ARRAY

bool operator <(const file_metadata f1, const file_metadata f2) {
    return f1.start_block < f2.start_block;
}

class textFS_tools {
public:

    void cmd_dispAllInodes(string cmd) {	//DISPLAY ALL FILES
        string argument;
        if (strlen(CMD_LS) + 1 < cmd.length() - 1)
            argument = cmd.substr(strlen(CMD_LS) + 1, cmd.length() - (strlen(CMD_LS) + 1));

        if (argument == "-l") { //DISPLAY FILES WITH FULL DETAILS
            cout << "[START]\t[ END ]\t[FILE]" << endl;
            cout << "[BLOCK]\t[BLOCK]\t[NAME]" << endl;
            for (int i = 0; i < file_table.size(); i++)
                cout << file_table[i].start_block << " \t" << file_table[i].length << " \t" <<
                file_table[i].filename << endl;
            if (file_table.size() == 0)
                cout << "No files present on drive..." << endl;
        }
        else {
            for (int i = 0; i < file_table.size(); i++)
                cout << file_table[i].filename << " \t";
            if (file_table.size() == 0)
                cout << "No files present on drive..." << endl;
            else
                cout << endl;
            cout << "Use ls -l more details..." << endl;
        }
    }

    string getBaseFilename(string argument) {
        int flg = argument.find("/");
        if (flg == -1)
            return argument;
        return getBaseFilename(argument.substr(flg + 1, argument.length() - flg - 1));
    }

    void cmd_copyFile(string cmd) {	//COPY EXTERNAL FILE TO INTERNAL
        string argument;
        if (strlen(CMD_COPY) + 1 < cmd.length())
            argument = cmd.substr(strlen(CMD_COPY) + 1, cmd.length() - (strlen(CMD_COPY) + 1));

        int find = argument.find(" ");
        string srcfilename = argument, destfilename;
        if (find != -1) {
            srcfilename = argument.substr(0, find);
            destfilename = argument.substr(find + 1, argument.length() - find - 1);
        }
        ifstream in;
        in.open(srcfilename.c_str());
        char a;
        string basefilename = getBaseFilename(argument);
        if (basefilename == "") {
            cout << "Invalid filename" << endl;
            cout << "Usage: copy <src> [OPTIONAL <dest>]" << endl;
            cout << "<src> can be relative or absolute path" << endl;
            return;
        }
        if (find != -1) {
            file_metadata *file = getFile(destfilename);
            if (file) {
                cout << "Destination already exists....\nChange the name" << endl;
                return;
            }
        }
        else {
            file_metadata *file = getFile(basefilename);
            if (file) {
                cout <<
                "Destination file already exists....\nGive explicit filename for Destination" <<
                endl;
                return;
            }
            destfilename = basefilename;
        }


        string data = "";
        in.seekg(0, ios::end);
        int tmp = (in.tellg());

        if (tmp == -1) {
            cout << "Unable to access source file..." << endl;
            return;
        }
        in.seekg(0, ios::beg);
        cout << "Copying....";
        while (in.get(a))
            data += a;

        cout << "[DONE]" << endl;
        cout << tmp << " Byte(s) copied...." << endl;
        in.close();
        addNewFile(destfilename, data);
    }

    int getFreeMemory(int len) {		//ALLOCATING Memory
        int start_block = 0;
        int inodes = file_table.size();
        if (inodes == 0)
            return start_block;
        else if (inodes > 0) {
            if (file_table[0].start_block >= len)
                return file_table[0].start_block - len;
        }

        int tmplen;
        for (int i = 1; i < inodes; i++) {
            tmplen = (file_table[i].start_block) -
                     (file_table[i - 1].start_block + file_table[i - 1].length);
            cout << tmplen << " " << len << endl;
            if (tmplen >= len)
                return (file_table[i - 1].start_block + file_table[i - 1].length);
        }

        if (inodes > 0) {
            int l1 = file_table[inodes - 1].start_block + file_table[inodes - 1].length - 1;
            int l2 = HARD_DISK_SIZE - l1 - 1;
            if (l2 >= len)
                return l1 + 1;
        }
        return -1;
    }

    void addNewFile(string filename, string data) { //ADD NEW FILE MODULE
        int start_block = getFreeMemory(data.length());
        if (start_block == -1) {
            cout << "CREATE_FILE [ERROR] -> Not Enough Memory...." << endl;
            return;
        }

        //Adding into data
        for (int i = start_block, j = 0; i < start_block + data.length(); i++, j++)
            mydrive[i] = data[j];
        //Adding into header
        file_metadata head;
        head.start_block = start_block;
        head.length = data.length();
        head.id = file_table.size() + 1;
        strcpy(head.filename, filename.c_str());
        file_table.push_back(head);
        sort(file_table.begin(), file_table.end());
        cout << "File Created Successfully" << endl;
    }

    file_metadata *getFile(string filename) {
        for (int i = 0; i < file_table.size(); i++)
            if (!strcmp(file_table[i].filename, filename.c_str()))
                return &file_table[i];
        return NULL;
    }

    bool deleteFile(string filename) {  //DELETE FILE
        for (vector<file_metadata>::iterator it = file_table.begin(); it < file_table.end(); it++)
            if (!strcmp((*it).filename, filename.c_str())) {
                file_table.erase(it);
                return true;
            }
        return false;
    }

    void cmd_createFile(string cmd) { //CREATE FILE
        string filename;
        if (strlen(CMD_CREATE) + 1 < cmd.length())
            filename = cmd.substr(strlen(CMD_CREATE) + 1, cmd.length() - (strlen(CMD_CREATE) + 1));

        if (filename.length() == 0) {
            cout << "Invalid Filename..." << endl;
            cout << "USAGE: create <filename>\n" << endl;
        }
        else if (getFile(filename))
            cout << "File already exists..." << endl;
        else {
            string data;
            cout << "Enter the data to be entered in file:" << endl;
            getline(cin, data);
            addNewFile(filename, data);
        }
    }

    void cmd_echoFile(string cmd) { //ECHO FILE
        string filename;
        if (strlen(CMD_ECHO) + 1 < cmd.length())
            filename = cmd.substr(strlen(CMD_ECHO) + 1, cmd.length() - (strlen(CMD_ECHO) + 1));

        if (filename.length() == 0) {
            cout << "Invalid Filename..." << endl;
            cout << "USAGE: echo <filename>\n" << endl;
        }
        else {
            file_metadata *file = getFile(filename);
            if (file) {
                int i;
                for (i = file->start_block; i < file->start_block + file->length; i++) {
                    cout << mydrive[i];
                    if (i + 1 == file->start_block + file->length && mydrive[i] != '\n')
                        cout << endl;
                }
            }
            else
                cout << "No such file exists..." << endl;
        }
    }

    void cmd_deleteFile(string cmd) { //DELETE FILE
        string filename;
        if (strlen(CMD_DELETE) + 1 < cmd.length())
            filename = cmd.substr(strlen(CMD_DELETE) + 1, cmd.length() - (strlen(CMD_DELETE) + 1));

        if (filename.length() == 0) {
            cout << "Invalid Filename..." << endl;
            cout << "USAGE: delete <filename>\n" << endl;
        }
        else {
            if (deleteFile(filename))
                cout << "File deleted...." << endl;
            else
                cout << "No such file exists..." << endl;
        }
    }

    void cmd_writeToDrive() { //WRITE FILE
        cout << "Writing your file system...";
        ofstream out;
        out.open(DRIVE_NAME, ios::binary);
        inode_info inf;
        inf.inodes = file_table.size();
        out.write((char *) &inf, sizeof(inode_info));
        for (int i = 0; i < file_table.size(); i++)
            out.write((char *) &file_table[i], sizeof(file_metadata));
        cout << "[DONE]" << endl;

        cout << "Writing data to file system...";
        out.seekp(MAX_METADATA_SIZE, ios::beg);
        out.write(mydrive, MAX_DRIVEDATA_SIZE);
        out.close();
        cout << "[DONE]" << endl;
    }

    void cmd_formatDrive() { //FORMAT DRIVE
        cout << "Formatting your drive...";
        file_table.clear();
        cout << "[DONE]" << endl;
        cmd_writeToDrive();
    }

    void cmd_readFromDrive() { //READ DRIVE
        cout << "Loading your file system...";
        ifstream inFile;
        inFile.open(DRIVE_NAME, ios::binary);
        file_metadata tmp_file;

        inode_info inf;
        file_table.clear();
        if (!inFile.read((char *) &inf, sizeof(inode_info))) {
            cout << "[ERROR]" << endl;
            cmd_formatDrive();
            return;
        }
        for (int i = 0; i < inf.inodes; i++) {
            inFile.read((char *) &tmp_file, sizeof(tmp_file));
            file_table.push_back(tmp_file);
        }

        sort(file_table.begin(), file_table.end());
        cout << "[DONE]" << endl;

        cout << "Loading your drive...";
        inFile.seekg(MAX_METADATA_SIZE, ios::beg);
        inFile.read(mydrive, MAX_DRIVEDATA_SIZE);
        inFile.close();

        cout << "[DONE]" << endl;
    }
};

int main(int argc, char** argv) { //MAIN FUNCTION
    system("clear");
    cout << "Welcome to TextFS v1.0 [DNS^2]\n.\n.\n." << endl;
    textFS_tools textfs;
    textfs.cmd_readFromDrive();
    cout << "Command Line Interface is starting....[DONE]" << endl;
    cout << "Type help for available commands..." << endl;
    while (true) {
        string cmd;
        cout << "cmd> ";
        getline(cin, cmd);

        if (cmd == CMD_FORMAT)
            textfs.cmd_formatDrive();

        else if (cmd.length() >= strlen(CMD_LS) && cmd.substr(0, strlen(CMD_LS)) == CMD_LS)
            textfs.cmd_dispAllInodes(cmd);

        else if (cmd.length() >= strlen(CMD_CREATE) &&
                 cmd.substr(0, strlen(CMD_CREATE)) == CMD_CREATE)
            textfs.cmd_createFile(cmd);

        else if (cmd.length() >= strlen(CMD_ECHO) && cmd.substr(0, strlen(CMD_ECHO)) == CMD_ECHO)
            textfs.cmd_echoFile(cmd);

        else if (cmd.length() >= strlen(CMD_DELETE) &&
                 cmd.substr(0, strlen(CMD_DELETE)) == CMD_DELETE)
            textfs.cmd_deleteFile(cmd);

        else if (cmd.length() >= strlen(CMD_COPY) && cmd.substr(0, strlen(CMD_COPY)) == CMD_COPY)
            textfs.cmd_copyFile(cmd);

        else if (cmd == CMD_RESTORE_DRIVE)
            textfs.cmd_readFromDrive();

        else if (cmd == CMD_SAVE_DRIVE)
            textfs.cmd_writeToDrive();

        else if (cmd == CMD_EXIT)
            break;
        else {
			cout<<"Available Commands:"<<endl;
			cout<<CMD_FORMAT<<endl;
			cout<<CMD_CREATE<<endl;
			cout<<CMD_LS<<endl;
			cout<<CMD_ECHO<<endl;
			cout<<CMD_COPY<<endl;
			cout<<CMD_DELETE<<endl;
			cout<<CMD_SAVE_DRIVE<<endl;
			cout<<CMD_RESTORE_DRIVE<<endl;
			cout<<CMD_EXIT<<endl;
        }
    }
    return 0;
}
