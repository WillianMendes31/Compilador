#include <bits/stdc++.h>

using namespace std;

// Variáveis globais
map<string, int> variables; // Para armazenar as variáveis
map<int, string> program;   // Para armazenar o programa BASIC
int currentLine = 10;       // Controla a linha atual
bool haltProgram = false;   // Flag para parar o programa

// Função para dividir uma string
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) { // Ignora tokens vazios
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Função para processar múltiplos comandos na mesma linha, separados por ":"
vector<string> splitByColon(const string& str) {
    return split(str, ':');
}

// Função para processar a entrada de um arquivo
void loadProgram(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Erro ao abrir o arquivo!" << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        int lineNumber;
        string command;
        istringstream iss(line);
        iss >> lineNumber;
        getline(iss, command);
        program[lineNumber] = command;
        cout << "Carregando linha " << lineNumber << ": " << command << endl;
    }
}

// Função para comparar valores
bool evaluateCondition(int left, string op, int right) {
    if (op == "<=") return left <= right;
    if (op == ">=") return left >= right;
    if (op == "<") return left < right;
    if (op == ">") return left > right;
    if (op == "==") return left == right;
    if (op == "!=") return left != right;
    return false;
}

// Função para processar uma única linha de comando BASIC
bool processCommand(const string& command) {
    cout << "Executando comando: " << command << endl;

    vector<string> tokens = split(command, ' ');

    // Verificar se há tokens suficientes
    if (tokens.empty()) return false;

    // Remover comentários
    if (tokens[0] == "rem") {
        return false; // Ignora a linha de comentário
    }

    // Comando de saída
    if (tokens[0] == "print") {
        if (tokens.size() > 1 && tokens[1][0] == '"') {
            string output = command.substr(command.find("\"") + 1);
            output = output.substr(0, output.rfind("\""));
            cout << output << endl;
        } else if (tokens.size() > 1) {
            // Print de variável
            if (variables.find(tokens[1]) != variables.end()) {
                cout << variables[tokens[1]] << endl;
            } else {
                cout << "Erro: Variável não definida -> " << tokens[1] << endl;
            }
        }
        return false;
    }

    // Comando de entrada
    else if (tokens[0] == "input") {
        if (tokens.size() > 1) {
            int value;
            cout << "Digite " << tokens[1] << ": ";
            cin >> value;
            variables[tokens[1]] = value;
            cout << "Variável " << tokens[1] << " atribuída com o valor: " << value << endl;
        } else {
            cout << "Erro: Comando input mal formatado!" << endl;
        }
        return false;
    }

    // Atribuição de variável
    else if (tokens.size() > 2 && tokens[1] == "=") {
        if (tokens.size() >= 5 && (tokens[3] == "+" || tokens[3] == "-")) {
            if (variables.find(tokens[2]) != variables.end() && variables.find(tokens[4]) != variables.end()) {
                if (tokens[3] == "+") {
                    variables[tokens[0]] = variables[tokens[2]] + variables[tokens[4]];
                } else {
                    variables[tokens[0]] = variables[tokens[2]] - variables[tokens[4]];
                }
            } else {
                cout << "Erro: Variáveis indefinidas para a operação" << endl;
            }
        } else {
            try {
                variables[tokens[0]] = stoi(tokens[2]);
            } catch (...) {
                if (variables.find(tokens[2]) != variables.end()) {
                    variables[tokens[0]] = variables[tokens[2]]; // Copiar valor da variável
                } else {
                    cout << "Erro: Atribuição inválida -> " << tokens[0] << endl;
                }
            }
        }
        cout << "Variável " << tokens[0] << " atribuída com o valor: " << variables[tokens[0]] << endl;
        return false;
    }

    // Goto
    else if (tokens[0] == "goto") {
        if (tokens.size() > 1 && program.find(stoi(tokens[1])) != program.end()) {
            currentLine = stoi(tokens[1]);
            cout << "Indo para linha: " << currentLine << endl;
        } else {
            cout << "Erro: Linha " << tokens[1] << " não encontrada!" << endl;
            haltProgram = true; // Para o programa, pois a linha não existe
        }
        return true;
    }

    // Condicional if com operadores adicionais
    else if (tokens[0] == "if") {
        if (tokens.size() > 5 && variables.find(tokens[1]) != variables.end()) {
            int varValue = variables[tokens[1]];
            int compareValue = stoi(tokens[3]);

            cout << "Comparando " << tokens[1] << " (" << varValue << ") com " << compareValue << endl;

            if (evaluateCondition(varValue, tokens[2], compareValue)) {
                if (program.find(stoi(tokens[5])) != program.end()) {
                    currentLine = stoi(tokens[5]);
                    cout << "Condição verdadeira, indo para linha: " << currentLine << endl;
                    return true;
                } else {
                    cout << "Erro: Linha " << tokens[5] << " não encontrada!" << endl;
                    haltProgram = true;
                }
            }
        } else {
            cout << "Erro: Comando if mal formatado ou variável não encontrada." << endl;
        }
    }

    // Comando halt para parar a execução
    else if (tokens[0] == "halt") {
        haltProgram = true;
        cout << "Programa finalizado com o comando HALT." << endl;
        return true;
    }

    return false;
}

// Função para executar o programa
void runProgram() {
    while (program.find(currentLine) != program.end() && !haltProgram) {
        cout << " -----------------------------------------" << endl;
        cout << "Linha atual: " << currentLine << endl;
        string commandLine = program[currentLine];
        vector<string> commands = splitByColon(commandLine);
        bool lineChanged = false;

        for (const string& command : commands) {
            if (processCommand(command)) {
                lineChanged = true;
                break; // Sai do loop assim que um comando "goto" ou "halt" for executado
            }
        }

        if (!lineChanged) {
            currentLine += 10; // Avança para a próxima linha
        }
    }
}

int main() {
    // Carregar o programa BASIC a partir do arquivo
    loadProgram("programa.txt");

    // Executar o programa
    runProgram();
}
