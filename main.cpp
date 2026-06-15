#include <iostream>
#include <vector>
using namespace std;

// Stub temporal — tú lo defines así para poder trabajar
struct Voto {
    std::string votanteId;
    std::string opcion;
};

struct Block {
    int index;
    std::string previous_hash;
    std::vector<Voto> votos;
    int nonce;
    std::string current_hash;
};

//integrante 2: Blockhain, mineBlock(int dificultad), isChainValid()

//hasta que el integrante 1 escoja hash:
string calcularHash(Block* block) {}

class Blockchain {
    vector<Block*> chain;
    static Blockchain* instance;
    Blockchain() {}
public:
    bool isChainValid() {
        for (int i = 1; i < chain.size(); i++) {
            if (chain[i]->current_hash != calcularHash(chain[i])) { return false; }
            if (chain[i-1]->current_hash != chain[i]->previous_hash) { return false; }
        }
        return true;
    }
    void mineBlock(int dificultad, Block* block) {
        int nonce = 0;
        while (calcularHash(block) != string(dificultad, '0')) {
            nonce++;
            block->nonce = nonce;
        }
        block->current_hash = calcularHash(block);
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
        : nombre(nombre), blockchain(Blockchain::getInstance()) {}

    void update(Block nuevoBloque) override {
        if (calcularHash(&nuevoBloque) == nuevoBloque.current_hash) {
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

    return 0;
}
