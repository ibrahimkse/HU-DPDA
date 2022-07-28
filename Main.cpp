#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stack>

using namespace std;

void copyStack(stack<string> s, vector<string> &v)
{
    if (s.empty()){
        return;
    }
    string x = s.top();
    s.pop();
    copyStack(s,v);
    v.push_back(x);
    s.push(x);
}

int main(int argc, char *argv[]) {

    // Taking inputs
    string dpdaString = argv[1];
    ifstream dpdaFile(dpdaString);
    string inputString = argv[2];
    ifstream inputFile(inputString);
    string outputString = argv[3];
    ofstream outputFile(outputString);

    vector<vector<string>> dpdaVector;
    vector<vector<string>> inputVector;

    //taking file content
    string myText;
    while (getline (inputFile, myText)) {
        vector<string> inner1;
        stringstream ss1(myText);
        string parsed1;
        while (getline(ss1,parsed1,',')){
            inner1.push_back(parsed1);
        }
        inputVector.push_back(inner1);
    }
    inputFile.close();

    while (getline (dpdaFile, myText)) {
        vector<string> inner2;
        stringstream ss1(myText);
        string parsed1;
        while (getline(ss1,parsed1,',')){
            inner2.push_back(parsed1);
        }
        dpdaVector.push_back(inner2);
    }
    dpdaFile.close();

    // Converting dpda content
    vector<vector<string>> transitionVector;
    vector<string> inputAlphabet;
    vector<string> stackAlphabet;
    vector<string> allStates;
    vector<string> finalStates;
    string startingState;
    for (vector<string> v : dpdaVector) {
        if (v[0].rfind("T", 0) == 0){
            vector<string> transitionInner;
            transitionInner.push_back(v[0].substr(2,v[0].size() - 2));
            for (int i = 1; i < v.size() ; i++) {
                transitionInner.push_back(v[i]);
            }
            transitionVector.push_back(transitionInner);
        }
        if (v[0].rfind("Z", 0) == 0){
            stackAlphabet.push_back(v[0].substr(2));
            for (int i = 1; i < v.size() ; i++) {
                stackAlphabet.push_back(v[i]);
            }
        }
        if (v[0].rfind("A", 0) == 0){
            inputAlphabet.push_back(v[0].substr(2));
            for (int i = 1; i < v.size() ; i++) {
                inputAlphabet.push_back(v[i]);
            }
        }
        if (v[0].rfind("Q", 0) == 0){
            allStates.push_back(v[0].substr(2,v[0].size() - 2));
            for (int i = 1; i < v.size() ; i++) {
                if (v[i].find("=") != string::npos){
                    int pos1 = v[i].find("(");
                    int pos2 = v[i].find(")");
                    allStates.push_back(v[i].substr(0,(pos2-pos1-1)));
                    startingState = v[i].substr(pos1+1,(pos2-pos1-1));
                }
                else if (v[i].find("[") != string::npos){
                    finalStates.push_back(v[i].substr(1,v[i].size()-2));
                }
                else {
                    allStates.push_back(v[i]);
                }
            }
        }
    }

    //Checking error;
    for (vector<string> t: transitionVector) {
        string currentState = t[0];
        string readInput = t[1];
        string itemToPop = t[2];
        string nextState = t[3];
        string itemToPush = t[4];
        bool isExists;
        if  (find(allStates.begin(), allStates.end(), currentState) != allStates.end()) {
            isExists = true;
        } else {
            outputFile << "Error [1]:DPDA description is invalid!" << endl;
            exit(0);
        }
        if  (find(allStates.begin(), allStates.end(), nextState) != allStates.end()) {
            isExists = true;
        } else {
            outputFile << "Error [1]:DPDA description is invalid!" << endl;
            exit(0);
        }
        if  (find(inputAlphabet.begin(), inputAlphabet.end(), readInput) != inputAlphabet.end()) {
            isExists = true;
        } else {
            if (readInput == "e"){
                isExists = true;
            } else {
                outputFile << "Error [1]:DPDA description is invalid!" << endl;
                exit(0);
            }
        }
        if  (find(stackAlphabet.begin(), stackAlphabet.end(), itemToPop) != stackAlphabet.end()) {
            isExists = true;
        } else {
            if (itemToPop == "e"){
                isExists = true;
            } else {
                outputFile << "Error [1]:DPDA description is invalid!" << endl;
                exit(0);
            }
        }
        if  (find(stackAlphabet.begin(), stackAlphabet.end(), itemToPush) != stackAlphabet.end()) {
            isExists = true;
        } else {
            if (itemToPush == "e"){
                isExists = true;
            } else {
                outputFile << "Error [1]:DPDA description is invalid!" << endl;
                exit(0);
            }
        }
    }

    vector<string> printVector;
    // Main operation
    for (vector<string> inputs:inputVector) {
        //stack for all operations
        stack<string> stack;
        string currState = startingState;
        int inputCounter = 0;
        if (inputs.empty()){
            outputFile << "ACCEPT" << endl;
            outputFile << endl;
        }
        else{
            for (string currentInput: inputs) {
                bool inputFound = false;
                bool transitionFound = false;
                while (!inputFound){
                    bool stepDone = false;
                    for (vector<string> transition: transitionVector) {
                        if (stepDone){
                            break;
                        }
                        transitionFound = false;
                        if (transition[0] == currState ){
                            if (transition[1] == currentInput){
                                if ((!stack.empty() && transition[2] == stack.top()) || (stack.size() == 1 && stack.top() == "$" && transition[2] == "e") || (transition[2] == "e")){
                                    transitionFound = true;
                                    currState = transition[3];
                                    inputFound = true;
                                    inputCounter++;
                                    if (transition[2] != "e"){
                                        stack.pop();
                                    }
                                    if (transition[4] != "e"){
                                        stack.push(transition[4]);
                                    }
                                    outputFile << transition[0] << "," << transition[1] << "," << transition[2] << " => " << transition[3] << "," << transition[4] << " [STACK]:";
                                    printVector.clear();
                                    copyStack(stack,printVector);
                                    for (int i = 0; i < printVector.size(); i++) {
                                        outputFile << printVector[i];
                                        if (i != (printVector.size()-1)){
                                            outputFile << ",";
                                        }
                                    }
                                    outputFile << endl;
                                    stepDone = true;

                                }
                            }else if(transition[1] == "e"){
                                transitionFound = true;
                                currState = transition[3];
                                if (transition[2] != "e"){
                                    stack.pop();
                                }
                                if (transition[4] != "e"){
                                    stack.push(transition[4]);
                                }
                                outputFile << transition[0] << "," << transition[1] << "," << transition[2] << " => " << transition[3] << "," << transition[4] << " [STACK]:";
                                printVector.clear();
                                copyStack(stack,printVector);
                                for (int i = 0; i < printVector.size(); i++) {
                                    outputFile << printVector[i];
                                    if (i != (printVector.size()-1)){
                                        outputFile << ",";
                                    }
                                }
                                outputFile << endl;
                                stepDone = true;
                            }
                        }
                    }
                    if (!transitionFound){
                        outputFile << "REJECT" << endl;
                        outputFile << endl;
                        break;
                    }
                }
            }
            if (inputCounter == inputs.size()){
                for (vector<string> transition: transitionVector) {
                    if (transition[0] == currState ){
                        if (transition[1] == "e" ){
                            currState = transition[3];
                            if (transition[2] == "$" && stack.top() == "$"){
                                stack.pop();
                            }
                            if (transition[4] != "e"){
                                stack.push(transition[4]);
                            }
                            outputFile << transition[0] << "," << transition[1] << "," << transition[2] << " => " << transition[3] << "," << transition[4] << " [STACK]:";
                            printVector.clear();
                            copyStack(stack,printVector);
                            for (int i = 0; i < printVector.size(); i++) {
                                outputFile << printVector[i];
                                if (i != (printVector.size()-1)){
                                    outputFile << ",";
                                }
                            }
                            outputFile << endl;
                            inputCounter ++;
                            if (stack.empty() || stack.top() == "$"){
                                if  (find(finalStates.begin(), finalStates.end(), transition[3]) != finalStates.end()) {
                                    outputFile << "ACCEPT" << endl;
                                    outputFile << endl;
                                    currState = "";
                                } else {
                                    outputFile << "REJECT" << endl;
                                    outputFile << endl;
                                    currState = "";
                                }
                            }
                            else{
                                outputFile << "REJECT" << endl;
                                outputFile << endl;
                                currState = "";
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
