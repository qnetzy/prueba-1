#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

struct Pokemon {
	int id_num;
	char nombre[50];
	char tipo1[20];
	char tipo2[20];
	int puntaje_total;
	int puntos_vida;
	int fuerza;
	int blindaje;
	int fuerza_magica;
	int blindaje_magico;
	int agilidad;
	int epoca;
	bool mistico;
};

const char* lista_etiquetas[] = {
	"Nombre", "Tipo 1", "Tipo 2", "Numero Pokedex", "Total", "HP",
	"Ataque", "Defensa", "Ataque especial", "Defensa especial",
	"Velocidad", "Generacion", "Legendario"
};

const int CANTIDAD_CAMPOS = 13;
const char* ARCHIVO_BINARIO = "pokemon.dat";

bool copiarTexto(char* destino, int capacidad, const char* origen) {
	size_t longitud = strlen(origen);
	if (longitud >= static_cast<size_t>(capacidad)) {
		return false;
	}
	strcpy(destino, origen);
	return true;
}

bool parsearCSV(const char* linea, char columnas[CANTIDAD_CAMPOS][256]) {
	int indice_columna = 0;
	int indice_caracter = 0;
	bool comillas_activas = false;

	for (int i = 0; ; ++i) {
		char letra = linea[i];
		if (indice_columna >= CANTIDAD_CAMPOS) {
			return false;
		}
		if (letra == '"' ) {
			if (comillas_activas && linea[i + 1] == '"') {
				if (indice_caracter >= 255) {
					return false;
				}
				columnas[indice_columna][indice_caracter++] = '"';
				++i;
			} else {
				comillas_activas = !comillas_activas;
			}
		} else if ((letra == ',' && !comillas_activas) || letra == '\0') {
			if (indice_columna >= CANTIDAD_CAMPOS) {
				return false;
			}
			columnas[indice_columna][indice_caracter] = '\0';
			++indice_columna;
			indice_caracter = 0;
			if (letra == '\0') {
				break;
			}
		} else {
			if (indice_caracter >= 255) {
				return false;
			}
			columnas[indice_columna][indice_caracter++] = letra;
		}
	}
	return !comillas_activas && indice_columna == CANTIDAD_CAMPOS;
}

bool convertirEntero(const char* texto, int& valor) {
	char* fin;
	long numero = strtol(texto, &fin, 10);
	if (texto == fin) {
		return false;
	}
	while (*fin != '\0' && isspace(static_cast<unsigned char>(*fin))) {
		++fin;
	}
	if (*fin != '\0' || numero < numeric_limits<int>::min() ||
		numero > numeric_limits<int>::max()) {
		return false;
	}
	valor = static_cast<int>(numero);
	return true;
}

bool convertirBooleano(const char* texto, bool& valor) {
	char minusculas[16];
	size_t longitud = strlen(texto);
	if (longitud >= sizeof(minusculas)) {
		return false;
	}
	for (size_t i = 0; i <= longitud; ++i) {
		minusculas[i] = static_cast<char>(
			tolower(static_cast<unsigned char>(texto[i])));
	}
	if (strcmp(minusculas, "true") == 0) {
		valor = true;
		return true;
	}
	if (strcmp(minusculas, "false") == 0) {
		valor = false;
		return true;
	}
	return false;
}

void redimensionar(Pokemon*& pokemones, int& capacidad, int cantidad_necesaria) {
	int nueva_capacidad = capacidad == 0 ? 10 : capacidad;
	while (nueva_capacidad < cantidad_necesaria) {
		nueva_capacidad *= 2;
	}

	Pokemon* nuevos_pokemones = new Pokemon[nueva_capacidad];
	for (int i = 0; i < nueva_capacidad && i < capacidad; ++i) {
		nuevos_pokemones[i] = pokemones[i];
	}
	delete[] pokemones;
	pokemones = nuevos_pokemones;
	capacidad = nueva_capacidad;
}

void agregarPokemon(Pokemon*& pokemones, int& cantidad, int& capacidad,
	const Pokemon& pokemon) {
	if (cantidad == capacidad) {
		redimensionar(pokemones, capacidad, cantidad + 1);
	}
	pokemones[cantidad] = pokemon;
	++cantidad;
}

bool importarCSV(const char* ruta, Pokemon*& pokemones, int& cantidad,
	int& capacidad, int& filas_invalidas) {
	ifstream archivo(ruta);
	if (!archivo.is_open()) {
		return false;
	}

	char linea[2048];
	archivo.getline(linea, sizeof(linea));
	while (archivo.getline(linea, sizeof(linea))) {
		size_t longitud_linea = strlen(linea);
		if (longitud_linea > 0 && linea[longitud_linea - 1] == '\r') {
			linea[longitud_linea - 1] = '\0';
		}
		char columnas[CANTIDAD_CAMPOS][256];
		Pokemon pokemon = {};
		bool fila_valida = parsearCSV(linea, columnas);
		if (fila_valida) {
			fila_valida =
				convertirEntero(columnas[0], pokemon.id_num) &&
				copiarTexto(pokemon.nombre, sizeof(pokemon.nombre), columnas[1]) &&
				copiarTexto(pokemon.tipo1, sizeof(pokemon.tipo1), columnas[2]) &&
				copiarTexto(pokemon.tipo2, sizeof(pokemon.tipo2), columnas[3]) &&
				convertirEntero(columnas[4], pokemon.puntaje_total) &&
				convertirEntero(columnas[5], pokemon.puntos_vida) &&
				convertirEntero(columnas[6], pokemon.fuerza) &&
				convertirEntero(columnas[7], pokemon.blindaje) &&
				convertirEntero(columnas[8], pokemon.fuerza_magica) &&
				convertirEntero(columnas[9], pokemon.blindaje_magico) &&
				convertirEntero(columnas[10], pokemon.agilidad) &&
				convertirEntero(columnas[11], pokemon.epoca) &&
				convertirBooleano(columnas[12], pokemon.mistico);
		}
		if (fila_valida) {
			agregarPokemon(pokemones, cantidad, capacidad, pokemon);
		} else {
			++filas_invalidas;
		}
	}
	return !archivo.bad();
}

bool guardarArchivoBinario(const char* ruta, Pokemon* pokemones,
	int cantidad) {
	ofstream archivo(ruta, ios::binary | ios::out | ios::trunc);
	if (!archivo.is_open()) {
		return false;
	}

	for (int i = 0; i < cantidad; ++i) {
		streamoff desplazamiento =
			static_cast<streamoff>(i) * sizeof(Pokemon);
		archivo.seekp(desplazamiento, ios::beg);
		archivo.write(reinterpret_cast<char*>(&pokemones[i]),
			sizeof(Pokemon));
		if (!archivo) {
			return false;
		}
	}
	return true;
}

bool leerPokemonPorIndice(const char* ruta, int indice, Pokemon& pokemon) {
	ifstream archivo(ruta, ios::binary | ios::in);
	if (!archivo.is_open()) {
		return false;
	}

	streamoff desplazamiento =
		static_cast<streamoff>(indice) * sizeof(Pokemon);
	archivo.seekg(desplazamiento, ios::beg);
	if (!archivo) {
		return false;
	}
	archivo.read(reinterpret_cast<char*>(&pokemon), sizeof(Pokemon));
	return archivo.good();
}

int compararTexto(const char* texto_a, const char* texto_b) {
	int indice = 0;
	while (texto_a[indice] != '\0' && texto_b[indice] != '\0') {
		int letra_a = tolower(static_cast<unsigned char>(texto_a[indice]));
		int letra_b = tolower(static_cast<unsigned char>(texto_b[indice]));
		if (letra_a != letra_b) {
			return letra_a < letra_b ? -1 : 1;
		}
		++indice;
	}
	if (texto_a[indice] == texto_b[indice]) {
		return 0;
	}
	return texto_a[indice] == '\0' ? -1 : 1;
}

bool contieneTexto(const char* texto, const char* consulta) {
	if (*consulta == '\0') {
		return true;
	}
	for (int inicio = 0; texto[inicio] != '\0'; ++inicio) {
		int desplazamiento = 0;
		while (consulta[desplazamiento] != '\0' &&
			texto[inicio + desplazamiento] != '\0' &&
			tolower(static_cast<unsigned char>(texto[inicio + desplazamiento])) ==
				tolower(static_cast<unsigned char>(consulta[desplazamiento]))) {
			++desplazamiento;
		}
		if (consulta[desplazamiento] == '\0') {
			return true;
		}
	}
	return false;
}

int obtenerValorNumerico(const Pokemon& pokemon, int campo) {
	switch (campo) {
		case 3: return pokemon.id_num;
		case 4: return pokemon.puntaje_total;
		case 5: return pokemon.puntos_vida;
		case 6: return pokemon.fuerza;
		case 7: return pokemon.blindaje;
		case 8: return pokemon.fuerza_magica;
		case 9: return pokemon.blindaje_magico;
		case 10: return pokemon.agilidad;
		default: return pokemon.epoca;
	}
}

int compararCampo(const Pokemon& pokemon_a, const Pokemon& pokemon_b,
	int campo) {
	if (campo == 0) {
		return compararTexto(pokemon_a.nombre, pokemon_b.nombre);
	}
	if (campo == 1) {
		return compararTexto(pokemon_a.tipo1, pokemon_b.tipo1);
	}
	if (campo == 2) {
		return compararTexto(pokemon_a.tipo2, pokemon_b.tipo2);
	}
	if (campo == 12) {
		if (pokemon_a.mistico == pokemon_b.mistico) {
			return 0;
		}
		return pokemon_a.mistico ? 1 : -1;
	}
	int valor_a = obtenerValorNumerico(pokemon_a, campo);
	int valor_b = obtenerValorNumerico(pokemon_b, campo);
	if (valor_a == valor_b) {
		return 0;
	}
	return valor_a < valor_b ? -1 : 1;
}

void ordenarBurbuja(Pokemon* pokemones, int cantidad, int campo,
	bool ascendente) {
	for (int fin = cantidad - 1; fin > 0; --fin) {
		bool hubo_cambio = false;
		for (int i = 0; i < fin; ++i) {
			int comparacion = compararCampo(pokemones[i], pokemones[i + 1], campo);
			if ((ascendente && comparacion > 0) ||
				(!ascendente && comparacion < 0)) {
				Pokemon temporal = pokemones[i];
				pokemones[i] = pokemones[i + 1];
				pokemones[i + 1] = temporal;
				hubo_cambio = true;
			}
		}
		if (!hubo_cambio) {
			break;
		}
	}
}

int solicitarEntero(const char* mensaje, int minimo, int maximo) {
	char entrada[128];
	while (true) {
		cout << mensaje;
		cin.getline(entrada, sizeof(entrada));
		if (!cin) {
			if (cin.eof()) {
				return minimo;
			}
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "La entrada es demasiado larga.\n";
			continue;
		}

		int valor;
		if (convertirEntero(entrada, valor) && valor >= minimo &&
			valor <= maximo) {
			return valor;
		}
		cout << "Ingresa un numero entre " << minimo << " y " << maximo << ".\n";
	}
}

void solicitarTexto(const char* mensaje, char* destino, int capacidad) {
	while (true) {
		cout << mensaje;
		cin.getline(destino, capacidad);
		if (cin) {
			return;
		}
		if (cin.eof()) {
			destino[0] = '\0';
			return;
		}
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "La entrada es demasiado larga.\n";
	}
}

void limpiarPantallaConsola() {
	system("cls");
}

void desplegarCampos() {
	for (int i = 0; i < CANTIDAD_CAMPOS; ++i) {
		cout << i + 1 << ". " << lista_etiquetas[i] << "\n";
	}
}

void renderizarGrilla(const Pokemon* pokemones, int cantidad) {
	cout << left << setw(6) << "Nro" << setw(16) << "Nombre"
		 << setw(12) << "Tipo 1" << setw(12) << "Tipo 2"
		 << right << setw(7) << "Total" << setw(6) << "HP"
		 << setw(9) << "Ataque" << setw(10) << "Defensa"
		 << setw(10) << "Atq.Esp" << setw(10) << "Def.Esp"
		 << setw(10) << "Velocidad" << setw(6) << "Gen"
		 << setw(12) << "Legendario" << "\n";
	for (int i = 0; i < 126; ++i) {
		cout << '-';
	}
	cout << "\n";

	for (int i = 0; i < cantidad; ++i) {
		const Pokemon& actual = pokemones[i];
		cout << left << setw(6) << actual.id_num << setw(16) << actual.nombre
			 << setw(12) << (actual.tipo1[0] == '\0' ? "-" : actual.tipo1)
			 << setw(12) << (actual.tipo2[0] == '\0' ? "-" : actual.tipo2)
			 << right << setw(7) << actual.puntaje_total
			 << setw(6) << actual.puntos_vida << setw(9) << actual.fuerza
			 << setw(10) << actual.blindaje << setw(10) << actual.fuerza_magica
			 << setw(10) << actual.blindaje_magico << setw(10) << actual.agilidad
			 << setw(6) << actual.epoca
			 << setw(12) << (actual.mistico ? "Si" : "No") << "\n";
	}
	cout << "Resultados: " << cantidad << "\n";
}

void ejecutarFiltro(const Pokemon* pokemones, int cantidad) {
	desplegarCampos();
	int campo = solicitarEntero("Campo para filtrar: ", 1, CANTIDAD_CAMPOS) - 1;
	Pokemon* filtrados = new Pokemon[cantidad];
	int cantidad_filtrada = 0;

	if (campo == 0) {
		char consulta[256];
		solicitarTexto("Texto a buscar (puede ser parcial): ", consulta,
			sizeof(consulta));
		for (int i = 0; i < cantidad; ++i) {
			if (contieneTexto(pokemones[i].nombre, consulta)) {
				filtrados[cantidad_filtrada++] = pokemones[i];
			}
		}
	} else if (campo == 1 || campo == 2) {
		char (*alternativas)[20] = new char[cantidad][20];
		int cantidad_alternativas = 0;
		for (int i = 0; i < cantidad; ++i) {
			const char* tipo = campo == 1 ? pokemones[i].tipo1 : pokemones[i].tipo2;
			bool encontrado = false;
			for (int j = 0; j < cantidad_alternativas; ++j) {
				if (strcmp(alternativas[j], tipo) == 0) {
					encontrado = true;
					break;
				}
			}
			if (!encontrado) {
				copiarTexto(alternativas[cantidad_alternativas], 20, tipo);
				++cantidad_alternativas;
			}
		}
		for (int fin = cantidad_alternativas - 1; fin > 0; --fin) {
			for (int i = 0; i < fin; ++i) {
				if (compararTexto(alternativas[i], alternativas[i + 1]) > 0) {
					char temporal[20];
					copiarTexto(temporal, sizeof(temporal), alternativas[i]);
					copiarTexto(alternativas[i], sizeof(alternativas[i]),
						alternativas[i + 1]);
					copiarTexto(alternativas[i + 1], sizeof(alternativas[i + 1]),
						temporal);
				}
			}
		}
		for (int i = 0; i < cantidad_alternativas; ++i) {
			cout << i + 1 << ". "
				 << (alternativas[i][0] == '\0' ? "Sin tipo secundario" :
					alternativas[i]) << "\n";
		}
		int seleccion = solicitarEntero("Elige un tipo: ", 1,
			cantidad_alternativas) - 1;
		for (int i = 0; i < cantidad; ++i) {
			const char* tipo = campo == 1 ? pokemones[i].tipo1 : pokemones[i].tipo2;
			if (strcmp(tipo, alternativas[seleccion]) == 0) {
				filtrados[cantidad_filtrada++] = pokemones[i];
			}
		}
		delete[] alternativas;
	} else if (campo == 12) {
		cout << "1. Legendario\n2. No legendario\n";
		bool legendario = solicitarEntero("Elige una opcion: ", 1, 2) == 1;
		for (int i = 0; i < cantidad; ++i) {
			if (pokemones[i].mistico == legendario) {
				filtrados[cantidad_filtrada++] = pokemones[i];
			}
		}
	} else {
		cout << "Usa el mismo minimo y maximo para buscar un valor exacto.\n";
		int minimo = solicitarEntero("Valor minimo: ", 0, 10000);
		int maximo = solicitarEntero("Valor maximo: ", minimo, 10000);
		for (int i = 0; i < cantidad; ++i) {
			int valor = obtenerValorNumerico(pokemones[i], campo);
			if (valor >= minimo && valor <= maximo) {
				filtrados[cantidad_filtrada++] = pokemones[i];
			}
		}
	}
	renderizarGrilla(filtrados, cantidad_filtrada);
	delete[] filtrados;
}

void ejecutarOrdenamiento(const Pokemon* pokemones, int cantidad) {
	desplegarCampos();
	int campo = solicitarEntero("Ordenar por campo: ", 1, CANTIDAD_CAMPOS) - 1;
	int sentido = solicitarEntero(
		"1. Menor a mayor / A-Z\n2. Mayor a menor / Z-A\nOpcion: ", 1, 2);
	Pokemon* ordenados = new Pokemon[cantidad];
	for (int i = 0; i < cantidad; ++i) {
		ordenados[i] = pokemones[i];
	}
	ordenarBurbuja(ordenados, cantidad, campo, sentido == 1);
	renderizarGrilla(ordenados, cantidad);
	delete[] ordenados;
}

void mostrarRankingExtremos(const Pokemon* pokemones, int cantidad) {
	Pokemon* ordenados = new Pokemon[cantidad];
	for (int i = 0; i < cantidad; ++i) {
		ordenados[i] = pokemones[i];
	}
	ordenarBurbuja(ordenados, cantidad, 4, true);

	int limite = cantidad < 3 ? cantidad : 3;
	Pokemon* debiles = new Pokemon[limite];
	Pokemon* fuertes = new Pokemon[limite];
	for (int i = 0; i < limite; ++i) {
		debiles[i] = ordenados[i];
		fuertes[i] = ordenados[cantidad - 1 - i];
	}
	cout << "Top 3 Pokemon mas debiles (menor Total):\n";
	renderizarGrilla(debiles, limite);
	cout << "\nTop 3 Pokemon mas fuertes (mayor Total):\n";
	renderizarGrilla(fuertes, limite);
	delete[] fuertes;
	delete[] debiles;
	delete[] ordenados;
}

Pokemon* buscarPokemonPorID(Pokemon* pokemones, int cantidad, int id_buscado) {
	for (int i = 0; i < cantidad; ++i) {
		if (pokemones[i].id_num == id_buscado) {
			return &pokemones[i];
		}
	}
	return nullptr;
}

void ejecutarComparativa(Pokemon* pokemones, int cantidad) {
	int id_primero = solicitarEntero("Numero Pokedex del primer Pokemon: ", 1, 150);
	int id_segundo = solicitarEntero("Numero Pokedex del segundo Pokemon: ", 1, 150);
	Pokemon* sujeto_a = buscarPokemonPorID(pokemones, cantidad, id_primero);
	Pokemon* sujeto_b = buscarPokemonPorID(pokemones, cantidad, id_segundo);

	if (sujeto_a == nullptr || sujeto_b == nullptr) {
		cout << "No se encontro uno de los Pokemon en el CSV.\n";
		return;
	}

	Pokemon par_seleccionado[2] = {*sujeto_a, *sujeto_b};
	renderizarGrilla(par_seleccionado, 2);
	cout << "Comparacion de stats (gana el valor mas alto):\n";
	const int arreglo_stats[] = {4, 5, 6, 7, 8, 9, 10};
	for (int i = 0; i < 7; ++i) {
		int campo = arreglo_stats[i];
		int puntaje_a = obtenerValorNumerico(*sujeto_a, campo);
		int puntaje_b = obtenerValorNumerico(*sujeto_b, campo);
		cout << lista_etiquetas[campo] << ": ";
		if (puntaje_a == puntaje_b) {
			cout << "empate (" << puntaje_a << ")\n";
		} else if (puntaje_a > puntaje_b) {
			cout << sujeto_a->nombre << " (" << puntaje_a << ")\n";
		} else {
			cout << sujeto_b->nombre << " (" << puntaje_b << ")\n";
		}
	}
}

int main() {
	Pokemon* pokemones = nullptr;
	int cantidad = 0;
	int capacidad = 0;
	int filas_invalidas = 0;
	char ruta_csv[260] = "pokemon_150_registros.csv";

	if (!importarCSV(ruta_csv, pokemones, cantidad, capacidad, filas_invalidas)) {
		cout << "No encontre " << ruta_csv << " en la carpeta actual.\n";
		solicitarTexto("Escribe la ruta del archivo CSV: ", ruta_csv,
			sizeof(ruta_csv));
		if (ruta_csv[0] == '\0' ||
			!importarCSV(ruta_csv, pokemones, cantidad, capacidad, filas_invalidas)) {
			cout << "No se pudo abrir el archivo CSV.\n";
			delete[] pokemones;
			return 1;
		}
	}
	if (cantidad == 0) {
		cout << "El CSV no contiene registros validos.\n";
		delete[] pokemones;
		return 1;
	}
	if (filas_invalidas > 0) {
		cout << "Se ignoraron " << filas_invalidas << " filas invalidas del CSV.\n";
	}

	while (true) {
		limpiarPantallaConsola();
		cout << "Se cargaron " << cantidad << " Pokemon.\n"
			 << "\n--- POKEDEX ---\n"
			 << "1. Mostrar todos\n"
			 << "2. Filtrar por campo\n"
			 << "3. Ordenar por campo\n"
			 << "4. Comparar dos Pokemon\n"
			 << "5. Ver Top 3 fuertes y debiles\n"
			 << "6. Guardar registros en archivo binario\n"
			 << "7. Consultar Pokemon por indice del archivo binario\n"
			 << "8. Salir\n";
		int opcion = solicitarEntero("Opcion: ", 1, 8);
		limpiarPantallaConsola();

		if (opcion == 1) {
			renderizarGrilla(pokemones, cantidad);
		} else if (opcion == 2) {
			ejecutarFiltro(pokemones, cantidad);
		} else if (opcion == 3) {
			ejecutarOrdenamiento(pokemones, cantidad);
		} else if (opcion == 4) {
			ejecutarComparativa(pokemones, cantidad);
		} else if (opcion == 5) {
			mostrarRankingExtremos(pokemones, cantidad);
		} else if (opcion == 6) {
			if (guardarArchivoBinario(ARCHIVO_BINARIO, pokemones, cantidad)) {
				cout << "Se guardaron " << cantidad << " registros en "
					 << ARCHIVO_BINARIO << ".\n";
			} else {
				cout << "No se pudo guardar el archivo binario.\n";
			}
		} else if (opcion == 7) {
			int indice = solicitarEntero("Indice del Pokemon (inicia en 1): ",
				1, cantidad);
			Pokemon pokemon;
			if (leerPokemonPorIndice(ARCHIVO_BINARIO, indice - 1, pokemon)) {
				renderizarGrilla(&pokemon, 1);
			} else {
				cout << "No se pudo leer el registro. Guarda primero el archivo "
					 << "binario con la opcion 6.\n";
			}
		} else {
			break;
		}
		cout << "\nPresiona Enter para volver al menu...";
		char pausa[2];
		cin.getline(pausa, sizeof(pausa));
		if (!cin) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	delete[] pokemones;
	return 0;
}
