#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

struct Voto {
    string idVotante;
    string opcion;

    // MEJORA A: Parametros pasados por const-ref en lugar de por valor,
    // evitando copias innecesarias de los strings al construir un Voto.
    Voto(const string& id, const string& op) {
        idVotante = id;
        opcion = op;
    }
};

// MEJORA A: Parametro "texto" pasado por const-ref en lugar de por valor,
// evitando copiar el string completo cada vez que se llama a generarHash().
string generarHash(const string& texto) {
    unsigned long long hash = 0;

    for (char c : texto) {
        hash = hash * 31 + c;
    }

    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}

class Block {
// MEJORA B: Los atributos pasan de publicos a privados para proteger
// el estado interno del bloque y evitar modificaciones externas directas.
private:
    int index;
    string previousHash;
    vector<Voto> votos;
    int nonce;
    string currentHash;

public:
    // MEJORA A: Parametros "hashAnterior" y "listaVotos" pasados por const-ref
    // en lugar de por valor, evitando copias al construir cada bloque.
    // MEJORA B: Se mantiene el constructor publico para poder crear bloques.
    Block(int idx, const string& hashAnterior, const vector<Voto>& listaVotos) {
        index = idx;
        previousHash = hashAnterior;
        votos = listaVotos;
        nonce = 0;
        currentHash = calcularHash();
    }

    // MEJORA B: Getters publicos para acceder a los atributos privados
    // desde Blockchain, MesaElectoral y cualquier otra clase que lo necesite.
    int           getIndex()        const { return index; }
    string        getPreviousHash() const { return previousHash; }
    string        getCurrentHash()  const { return currentHash; }
    int           getNonce()        const { return nonce; }
    vector<Voto>  getVotos()        const { return votos; }

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
        // MEJORA B: Se usan los getters en lugar de acceder directo al atributo.
        cout << "Bloque #" << getIndex() << " fue minado correctamente." << endl;
        cout << "Nonce encontrado: " << getNonce() << endl;
        cout << "Hash final: " << getCurrentHash() << endl;
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

        // MEJORA B: Se usan getters en lugar de acceso directo a atributos publicos.
        if (bloque.getCurrentHash() != bloque.calcularHash()) {
            return false;
        }
        if (bloque.getCurrentHash().substr(0, dificultad) != prefijo) {
            return false;
        }
        if (bloque.getPreviousHash() != cadena.back().getCurrentHash()) {
            return false;
        }
        return true;
    }

    bool cadenaEsValida() const {
        for (int i = 1; i < cadena.size(); i++) {
            // MEJORA B: Se usan getters en lugar de acceso directo a atributos publicos.
            if (cadena[i].getCurrentHash() != cadena[i].calcularHash()) {
                return false;
            }
            if (cadena[i].getPreviousHash() != cadena[i - 1].getCurrentHash()) {
                return false;
            }
        }
        return true;
    }

    void mostrarCadena() const {
        for (const Block& bloque : cadena) {
            // MEJORA B: Se usan getters en lugar de acceso directo a atributos publicos.
            cout << "\nBloque #" << bloque.getIndex() << endl;
            cout << "Hash anterior: " << bloque.getPreviousHash() << endl;
            cout << "Hash actual: " << bloque.getCurrentHash() << endl;
            cout << "Nonce: " << bloque.getNonce() << endl;
            if (bloque.getVotos().empty()) {
                cout << "Bloque inicial sin votos." << endl;
            } else {
                cout << "Votos registrados:" << endl;
                for (const Voto& voto : bloque.getVotos()) {
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
    // MEJORA A: Parametro "nombreMesa" pasado por const-ref en lugar de por valor.
    MesaElectoral(const string& nombreMesa, int dificultadRed) {
        nombre = nombreMesa;
        dificultad = dificultadRed;
    }

    // MEJORA A: Parametro "votos" pasado por const-ref en lugar de por valor,
    // evitando copiar el vector completo de votos en cada llamada.
    Block registrarVotos(const vector<Voto>& votos) {
        Block ultimoBloque = blockchain.obtenerUltimoBloque();

        // MEJORA B: Se usan getters para leer los datos del ultimo bloque.
        Block nuevoBloque(
            ultimoBloque.getIndex() + 1,
            ultimoBloque.getCurrentHash(),
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
