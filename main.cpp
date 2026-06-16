#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

// Integrante 1 : Clase Voto, y Clase Block con hash simple y funcion calcularHash
class Voto {
    string votanteId;
    string opcion;
public:
    Voto(const string& nombreVotante, const string& opcion) {
        size_t h = hash<string>{}(nombreVotante);
        votanteId = to_string(h % 1000000);
        this->opcion = opcion;
    }

    string getVotanteID() const {
        return votanteId;
    }

    string getOpcion() const {
        return opcion;
    }

    string toString() const {
        return votanteId + opcion;
    }
};

struct Block {
    int index;
    string previous_hash;
    vector<Voto> votos;
    int nonce;
    string current_hash;
public:
    Block() {}
    Block(int index,
          const string& previous_hash,
          const vector<Voto>& votos): index(index),
          previous_hash(previous_hash),
          votos(votos),
          nonce(0) {
        current_hash = calcularHash();
    }
    string calcularHash(){
        string datos;
        datos += to_string(index);
        datos += previous_hash;
        datos += to_string(nonce);
        for (const auto& voto : votos) {
            datos += voto.toString();
        }
        size_t hashValue = hash<string>{}(datos);

        stringstream ss;
        ss << setw(16) << setfill('0') << hex << hashValue;
        return ss.str();
}
};

//integrante 2: Blockhain, mineBlock(int dificultad), isChainValid()

class Blockchain {
    vector<Block*> chain;
    static Blockchain* instance;
public:
    Blockchain() {}
    bool isChainValid() {
        for (int i = 1; i < chain.size(); i++) {
            if (chain[i]->current_hash != chain[i]->calcularHash()) { return false; }
            if (chain[i-1]->current_hash != chain[i]->previous_hash) { return false; }
        }
        return true;
    }

    void agregarBloque(Block* bloque) {
        chain.push_back(bloque);
    }

    void mineBlock(int dificultad, Block* block) {
        int nonce = 0;
        while (block->calcularHash().substr(0, dificultad) != string(dificultad, '0')) {

            nonce++;
            block->nonce = nonce;
        }
        block->current_hash = block->calcularHash();
    }
    static Blockchain* getInstance() {
        if (instance == nullptr) { instance = new Blockchain(); }
        return instance;
    }
};
Blockchain* Blockchain::instance = nullptr;


class MesaElectoralObserver {
public:
    virtual void update(Block nuevoBloque) = 0;
    virtual ~MesaElectoralObserver() = default;
};

class MesaElectoral : public MesaElectoralObserver {
    string nombre;
    Blockchain* blockchain;
public:
    MesaElectoral(string nombre)
        : nombre(nombre), blockchain(new Blockchain()) {}

    void update(Block nuevoBloque) override {
        if (nuevoBloque.calcularHash() == nuevoBloque.current_hash) {
            blockchain->agregarBloque(new Block(nuevoBloque));
            cout << "[" << nombre << "] Bloque Num. " << nuevoBloque.index
                 << " ACEPTADO | Hash: " << nuevoBloque.current_hash << "\n";
        } else {
            cout << "[" << nombre << "] Bloque Num. " << nuevoBloque.index
                 << " RECHAZADO \n";
        }
    }

    void mostrarEstado() {
        cout << "[" << nombre << "] Cadena valida: "
             << (blockchain->isChainValid() ? "SI" : "NO") << "\n";
    }
};

class CentroElectoralSubject {
    vector<MesaElectoralObserver*> mesas;
public:
    void attach(MesaElectoralObserver* mesa) {
        mesas.push_back(mesa);
    }

    void notificarNuevoBloque(Block bloque) {
        for (auto mesa : mesas) {
            mesa->update(bloque);
        }
    }
};

int main() {
    MesaElectoral mesa1("Mesa-1");
    MesaElectoral mesa2("Mesa-2");
    MesaElectoral mesa3("Mesa-3");

    CentroElectoralSubject centro;
    centro.attach(&mesa1);
    centro.attach(&mesa2);
    centro.attach(&mesa3);

    Block bloque;
    bloque.index = 1;
    bloque.previous_hash = "0000000000";
    bloque.votos = {{"ID_01", "Candidato A"}, {"ID_02", "Candidato B"}};
    bloque.nonce = 0;

    cout << "=== Minando bloque #1 ===\n";
    Blockchain::getInstance()->mineBlock(3, &bloque);
    cout << "Nonce encontrado: " << bloque.nonce
         << " | Hash: " << bloque.current_hash << "\n\n";

    cout << "=== Notificando a la red ===\n";
    centro.notificarNuevoBloque(bloque);

    cout << "\n=== Estado final de la red ===\n";
    mesa1.mostrarEstado();
    mesa2.mostrarEstado();
    mesa3.mostrarEstado();
    // Cada MesaElectoral simula un nodo independiente (en la realidad seria un proceso aparte
    // con su propio Singleton de Blockchain). Como la demo corre las 3 mesas en un solo main,
    // cada una mantiene su propia cadena para reflejar ese aislamiento entre nodos.
    return 0;
}