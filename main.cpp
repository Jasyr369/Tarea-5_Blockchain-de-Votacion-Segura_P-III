#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
struct Voto {
    string idVotante;
    string opcion;

    Voto(string id, string op) {
        idVotante = id;
        opcion = op;
    }
};
string generarHash(string texto) {
    unsigned long long hash = 0;

    for (char c : texto) {
        hash = hash * 31 + c;
    }

    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}
class Block {
public:
    int index;
    string previousHash;
    vector<Voto> votos;
    int nonce;
    string currentHash;
    Block(int idx, string hashAnterior, vector<Voto> listaVotos) {
        index = idx;
        previousHash = hashAnterior;
        votos = listaVotos;
        nonce = 0;
        currentHash = calcularHash();
    }
    string calcularHash() const {
        stringstream datos;
        datos << index << previousHash << nonce;
        for (const Voto& voto : votos) {
            datos << voto.idVotante << voto.opcion;
        }
        return generarHash(datos.str());
    }
    void mineBlock(int dificultad) {
        string prefijo(dificultad, '0');
        while (currentHash.substr(0, dificultad) != prefijo) {
            nonce++;
            currentHash = calcularHash();
        }
        cout << "Bloque #" << index << " fue minado correctamente." << endl;
        cout << "Nonce encontrado: " << nonce << endl;
        cout << "Hash final: " << currentHash << endl;
    }
};
class Blockchain {
private:
    vector<Block> cadena;
public:
    Blockchain() {
        vector<Voto> votosIniciales;
        Block bloqueGenesis(0, "0", votosIniciales);
        cadena.push_back(bloqueGenesis);
    }
    Block obtenerUltimoBloque() const {
        return cadena.back();
    }
    void agregarBloque(Block bloque) {
        cadena.push_back(bloque);
    }
    bool verificarBloque(Block bloque, int dificultad) const {
        string prefijo(dificultad, '0');

        if (bloque.currentHash != bloque.calcularHash()) {
            return false;
        }
        if (bloque.currentHash.substr(0, dificultad) != prefijo) {
            return false;
        }
        if (bloque.previousHash != cadena.back().currentHash) {
            return false;
        }
        return true;
    }
    bool cadenaEsValida() const {
        for (int i = 1; i < cadena.size(); i++) {
            if (cadena[i].currentHash != cadena[i].calcularHash()) {
                return false;
            }
            if (cadena[i].previousHash != cadena[i - 1].currentHash) {
                return false;
            }
        }
        return true;
    }
    void mostrarCadena() const {
        for (const Block& bloque : cadena) {
            cout << "\nBloque #" << bloque.index << endl;
            cout << "Hash anterior: " << bloque.previousHash << endl;
            cout << "Hash actual: " << bloque.currentHash << endl;
            cout << "Nonce: " << bloque.nonce << endl;
            if (bloque.votos.empty()) {
                cout << "Bloque inicial sin votos." << endl;
            } else {
                cout << "Votos registrados:" << endl;
                for (const Voto& voto : bloque.votos) {
                    cout << "- " << voto.idVotante << " voto por "
                         << voto.opcion << endl;
                }
            }
        }
    }
};
class MesaElectoralObserver {
public:
    virtual void update(Block nuevoBloque) = 0;
    virtual ~MesaElectoralObserver() {}
};
class MesaElectoral : public MesaElectoralObserver {
private:
    string nombre;
    Blockchain blockchain;
    int dificultad;

public:
    MesaElectoral(string nombreMesa, int dificultadRed) {
        nombre = nombreMesa;
        dificultad = dificultadRed;
    }
    Block registrarVotos(vector<Voto> votos) {
        Block ultimoBloque = blockchain.obtenerUltimoBloque();

        Block nuevoBloque(
            ultimoBloque.index + 1,
            ultimoBloque.currentHash,
            votos
        );
        cout << "\n" << nombre << " esta minando un nuevo bloque..." << endl;
        nuevoBloque.mineBlock(dificultad);
        blockchain.agregarBloque(nuevoBloque);
        return nuevoBloque;
    }
    void update(Block nuevoBloque) override {
        cout << "\n" << nombre << " recibio un bloque nuevo." << endl;
        if (blockchain.verificarBloque(nuevoBloque, dificultad)) {
            blockchain.agregarBloque(nuevoBloque);
            cout << nombre << " verifico el bloque y lo agrego a su cadena." << endl;
        } else {
            cout << nombre << " rechazo el bloque porque no era valido." << endl;
        }
    }
    void mostrarBlockchain() const {
        cout << "\n==============================" << endl;
        cout << nombre << endl;
        cout << "==============================" << endl;
        blockchain.mostrarCadena();
    }
    bool estaCorrecta() const {
        return blockchain.cadenaEsValida();
    }
};
class CentroElectoralSubject {
private:
    vector<MesaElectoralObserver*> mesasRegistradas;
public:
    void attach(MesaElectoralObserver* mesa) {
        mesasRegistradas.push_back(mesa);
    }
    void notificarNuevoBloque(Block bloque, MesaElectoralObserver* mesaQueMino) {
        for (MesaElectoralObserver* mesa : mesasRegistradas) {
            if (mesa != mesaQueMino) {
                mesa->update(bloque);
            }
        }
    }
};
int main() {
    int dificultad = 2;
    CentroElectoralSubject redElectoral;
    MesaElectoral mesa1("Mesa Electoral 1", dificultad);
    MesaElectoral mesa2("Mesa Electoral 2", dificultad);
    MesaElectoral mesa3("Mesa Electoral 3", dificultad);
    redElectoral.attach(&mesa1);
    redElectoral.attach(&mesa2);
    redElectoral.attach(&mesa3);
    vector<Voto> votosMesa1;
    votosMesa1.push_back(Voto("ID_01", "Candidato A"));
    votosMesa1.push_back(Voto("ID_02", "Candidato B"));
    votosMesa1.push_back(Voto("ID_03", "Candidato A"));
    Block bloqueMinado = mesa1.registrarVotos(votosMesa1);
    cout << "\nLa Mesa Electoral 1 termino de minar el bloque." << endl;
    cout << "Ahora la red avisa automaticamente a las demas mesas." << endl;
    redElectoral.notificarNuevoBloque(bloqueMinado, &mesa1);
    mesa1.mostrarBlockchain();
    mesa2.mostrarBlockchain();
    mesa3.mostrarBlockchain();
    cout << "\nResultado de la validacion final:" << endl;
    cout << "Mesa 1: " << (mesa1.estaCorrecta() ? "cadena valida" : "cadena invalida") << endl;
    cout << "Mesa 2: " << (mesa2.estaCorrecta() ? "cadena valida" : "cadena invalida") << endl;
    cout << "Mesa 3: " << (mesa3.estaCorrecta() ? "cadena valida" : "cadena invalida") << endl;
    return 0;
}