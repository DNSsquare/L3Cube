#include <bits/stdc++.h>
using namespace std;

vector <string> data;
void getAllVersions(int argument);

class svc_commit_control {
public:
    int mode;    //0:Add new line 1:Delete line
    int current_file_pointer;
    int delIndex;
    char dat[21];

    svc_commit_control() { }

    svc_commit_control(char *argv) {
        ifstream in;
        ofstream out;
        char filename[20];
        strcpy(filename, "svc_");
        strcat(filename, argv);
        strcat(filename, ".dns2");
        in.open(filename, ios::binary | ios::ate);
        ifstream src;
        src.open(argv, ios::ate);
        if (in.tellg() == -1) {
            mode = 0;
            current_file_pointer = src.tellg();
            src.seekg(0, ios::beg);
            src.read(dat, current_file_pointer);
        }
        else {
            in.seekg(-1 * sizeof(*this), ios::end);
            svc_commit_control s;
            in.read((char *) &s, sizeof(svc_commit_control));
            int pointer = src.tellg();
            if (s.current_file_pointer < pointer) {
                // new line added
                mode = 0;
                char tmpdata[21];
                src.seekg(s.current_file_pointer + 1, ios::beg);
                src.read(tmpdata, pointer - s.current_file_pointer);
                strcpy(this->dat, tmpdata);
                current_file_pointer = pointer;
            }
            else if (s.current_file_pointer > pointer) {
                //deleted
                mode = 1;
                getAllVersions(-1);
                src.seekg(0, ios::beg);

                char buff[21];
                for (int i = 0; i < data.size(); i++) {
                    src.read(buff, data[i].length() + 1);
                    stringstream s1(buff);
                    stringstream s2(data[i]);
                    string ss1 = s1.str();
                    string ss2 = s2.str();
                    if (ss1 != ss2) {
                        delIndex = i;
                        break;
                    }
                }
                current_file_pointer = pointer;
            }
            else
                mode = 2;
        }
        src.close();
        in.close();
    }
};

void getAllVersions(int argument) {
    data.clear();
    if (argument == -1)
        argument = 100;
    ifstream svc_history;
    char filename[20];
    svc_history.open("svc_history");
    svc_history.read(filename, sizeof(filename));
    svc_history.close();

    ifstream in;
    in.open(filename, ios::binary);
    while (argument-- >= 0) {
        svc_commit_control tmp;
        if (!in.read((char *) &tmp, sizeof(svc_commit_control)))break;
        if (tmp.mode == 0) {
            stringstream ss(tmp.dat);
            data.push_back(ss.str());
        }
        else if (tmp.mode == 1) {
            vector<string>::iterator it = data.begin() + tmp.delIndex;
            // cout<<"deleted "<<data[tmp.delIndex]<<endl;
            data.erase(it);
        }
    }
}


int parseInt(char *str) {
    for (int i = 0; str[i] != 0; i++)
        if (str[i] < '0' || str[i] > '9')
            return -1;
    return atoi(str);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "1 argument needed..." << endl;
        cout << "Usage: svc <filename>  >> Commits the file" << endl;
        cout << "       svc <integer> " << endl;
        return 0;
    }

    int argument = parseInt(argv[1]);
    if (argument == -1) {
        svc_commit_control *s = new svc_commit_control(argv[1]);
        char filename[20];
        strcpy(filename, "svc_");
        strcat(filename, argv[1]);
        strcat(filename, ".dns2");
        if (s->mode != 2) {
            ofstream out;
            out.open(filename, ios::binary | ios::ate | ios::app);
            out.write((char *) s, sizeof(svc_commit_control));
            out.close();
        }

        ofstream svc_history;
        svc_history.open("svc_history", ios::trunc);
        svc_history.write(filename, sizeof(filename));
        svc_history.close();
    }
    else {
        getAllVersions(argument);
        for (int i = 0; i < data.size() - 1; i++)
            cout << data[i] << endl;
        if (data.size() > 0)
            cout << data[data.size() - 1] << endl;
    }
    return 0;
}