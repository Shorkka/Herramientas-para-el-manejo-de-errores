/*
Nombre del programador: Araujo Barron Yared Isaac (Shorkka)
Nombre del programa: Adivina las cartas.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <cassert>
#include <locale>
#include <clocale>

using namespace std;

// Solución para compiladores que no soportan <thread>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void sleepForSeconds(int seconds) {
    #ifdef _WIN32
    Sleep(seconds * 1000);
    #else
    sleep(seconds);
    #endif
}

class MemoryGame {
private:
    vector<int> cards;
    vector<bool> revealed;
    vector<bool> matched;
    int pairsFound;
    int attempts;
    int failedAttempts;
    int totalPairs;

    void validateState() const {
        assert(cards.size() == static_cast<size_t>(totalPairs * 2) &&
               "Número incorrecto de cartas");
        assert(revealed.size() == cards.size() &&
               "Tamaño de vector revealed incorrecto");
        assert(matched.size() == cards.size() &&
               "Tamaño de vector matched incorrecto");
        assert(pairsFound >= 0 && pairsFound <= totalPairs &&
               "Número inválido de pares encontrados");
    }

    void initializeDeck() {
        cards.clear();
        for(int i = 1; i <= totalPairs; i++) {
            cards.push_back(i);
            cards.push_back(i);
        }

        random_device rd;
        mt19937 g(rd());
        shuffle(cards.begin(), cards.end(), g);

        revealed.assign(cards.size(), false);
        matched.assign(cards.size(), false);
        pairsFound = 0;
        attempts = 0;
        failedAttempts = 0;

        validateState();
    }

    void showAllCardsTemporarily() {
        system("cls || clear");
        cout << "=== JUEGO DE MEMORIA ===" << endl;
        cout << "Memoriza las cartas..." << endl << endl;

        for(size_t i = 0; i < cards.size(); i++) {
            cout << "[" << setw(2) << cards[i] << "] ";
            if((i+1) % 4 == 0) cout << endl;
        }
        cout << endl;

        sleepForSeconds(3);
    }

public:
    MemoryGame(int pairs) : totalPairs(pairs) {
        if(pairs < 2 || pairs > 10) {
            throw invalid_argument("El número de pares debe estar entre 2 y 10");
        }

        initializeDeck();
        showAllCardsTemporarily();
    }

    void displayBoard() const {
        system("cls || clear");
        cout << "=== JUEGO DE MEMORIA (Tolerante a Fallos) ===" << endl;
        cout << "Pares encontrados: " << pairsFound << "/" << totalPairs << endl;
        cout << "Pares restantes: " << (totalPairs - pairsFound) << endl;
        cout << "Intentos: " << attempts << endl;
        cout << "Errores: " << failedAttempts << endl << endl;

        for(size_t i = 0; i < cards.size(); i++) {
            if(matched[i]) {
                cout << "[XX] ";
            } else if(revealed[i]) {
                cout << "[" << setw(2) << cards[i] << "] ";
            } else {
                cout << "[" << setw(2) << (i+1) << "] ";
            }

            if((i+1) % 4 == 0) cout << endl;
        }
        cout << endl;
    }

    void play() {
        int firstCard = -1, secondCard = -1;

        while(pairsFound < totalPairs) {
            try {
                displayBoard();
                validateState();

                // Selección de primera carta
                cout << "Selecciona la primera carta (1-" << cards.size() << "): ";
                firstCard = getValidInput() - 1;
                validateCardSelection(firstCard);

                revealed[firstCard] = true;
                displayBoard();

                // Selección de segunda carta
                cout << "Selecciona la segunda carta (1-" << cards.size() << "): ";
                secondCard = getValidInput() - 1;
                validateCardSelection(secondCard);

                if(firstCard == secondCard) {
                    revealed[firstCard] = false;
                    throw logic_error("No puedes seleccionar la misma carta dos veces");
                }

                revealed[secondCard] = true;
                displayBoard();

                // Procesar selecciones
                if(cards[firstCard] == cards[secondCard]) {
                    cout << "¡Encontraste un par!" << endl;
                    pairsFound++;
                    matched[firstCard] = matched[secondCard] = true;
                } else {
                    cout << "No es un par. Intenta de nuevo." << endl;
                    failedAttempts++;
                    revealed[firstCard] = revealed[secondCard] = false;
                }
                attempts++;

                // Pausa para permitir ver el resultado
                cout << "Presiona Enter para continuar...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
            }
            catch(const exception& e) {
                cerr << "\nError controlado: " << e.what() << endl;
                cerr << "El juego continuará normalmente..." << endl;

                fill(revealed.begin(), revealed.end(), false);

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Presiona Enter para continuar...";
                cin.get();
            }
        }

        displayFinalResults();
    }

private:
    int getValidInput() {
        int input;
        cin >> input;

        if(cin.fail()) {
            throw runtime_error("Entrada inválida. Debes ingresar un número.");
        }

        return input;
    }

    void validateCardSelection(int cardIndex) const {
        if(cardIndex < 0 || cardIndex >= static_cast<int>(cards.size())) {
            throw out_of_range("Selección fuera de rango. Intenta con un número entre 1 y " +
                              to_string(cards.size()));
        }

        if(matched[cardIndex]) {
            throw logic_error("Esta carta ya fue emparejada y no puede seleccionarse.");
        }

        if(revealed[cardIndex]) {
            throw logic_error("Esta carta ya está revelada.");
        }
    }

    void displayFinalResults() const {
        displayBoard();
        cout << "¡Felicidades! Completaste el juego en " << attempts << " intentos." << endl;
        cout << "Errores cometidos: " << failedAttempts << endl;
        cout << "Eficiencia: " << fixed << setprecision(1)
             << (static_cast<float>(pairsFound) / attempts * 100) << "%" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "");

    try {
        cout << "=== BIENVENIDO AL JUEGO DE MEMORIA (Tolerante a Fallos) ===" << endl;
        cout << "Ingresa el número de pares a encontrar (2-10): ";

        int pairs;
        if(!(cin >> pairs)) {
            throw runtime_error("Entrada inválida. Debe ser un número.");
        }

        MemoryGame game(pairs);
        game.play();
    }
    catch(const exception& e) {
        cerr << "\nError crítico: " << e.what() << endl;
        cerr << "El juego debe terminarse. Reinicie la aplicación." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
