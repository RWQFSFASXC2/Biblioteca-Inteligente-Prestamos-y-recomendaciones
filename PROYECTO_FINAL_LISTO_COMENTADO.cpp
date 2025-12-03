// PROYECTO FINAL - ESTRUCTURA DE DATOS
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <cctype>
#include <random>

using namespace std;

// ---------------------------------------------------------------------------
// FUNCION: generar_id_aleatorio
// Genera un ID aleatorio de longitud 'len' usando únicamente dígitos 0-9.
// Se usa para crear IDs de usuario y parte del ISBN cuando se genera automático.
// ---------------------------------------------------------------------------
string generar_id_aleatorio(size_t len = 12) {
    static random_device rd;
    static default_random_engine generator(rd());
    
    const string caracteres = "0123456789";
    uniform_int_distribution<int> distribution(0, (int)caracteres.length() - 1);
    string random_id;
    for (size_t i = 0; i < len; ++i) {
        random_id += caracteres[distribution(generator)];
    }
    return random_id;
}

// ---------------------------------------------------------------------------
// FUNCION: trim
// Elimina espacios en blanco (inicio y final) de un string.
// Se utiliza para limpiar entradas del CSV o de consola.
// ---------------------------------------------------------------------------
string trim(const string& str) {
    const string whitespace = " \t\n\r";
    size_t first = str.find_first_not_of(whitespace);
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// ------------------ Trie -----------------
// Estructura de datos TRIE para soporte de autocompletado sobre cadenas.
// Se usa para autocompletar búsquedas de títulos y autores.
struct Trie {
    struct Nodo {
        // Mapa de caracter -> índice del siguiente nodo en el vector 'tabla'
        unordered_map<char, int> siguiente;
        // Indica si en este nodo termina una palabra completa
        bool fin = false;
    };
    // 'tabla' almacena todos los nodos del trie; el índice 0 es la raíz.
    vector<Nodo> tabla = { Nodo() };

    // Inserta una cadena 's' completa en el Trie.
    void insertar(const string& s) {
        int u = 0;
        for (char c : s) {
            if (!tabla[u].siguiente.count(c)) {
                tabla[u].siguiente[c] = (int)tabla.size();
                tabla.push_back(Nodo());
            }
            u = tabla[u].siguiente[c];
        }
        tabla[u].fin = true;
    }

    // --- Función auxiliar recursiva para recolectar palabras ---
    // Recorre en profundidad a partir de un nodo 'u' y va acumulando caracteres
    // en 'prefijo_actual', agregando al vector 'resultados' todas las palabras completas.
    void recolectar(int u, string prefijo_actual, vector<string>& resultados) const {
        if (tabla[u].fin) {
            resultados.push_back(prefijo_actual);
        }
        for (auto const& [key, next_node_index] : tabla[u].siguiente) {
            recolectar(next_node_index, prefijo_actual + key, resultados);
        }
    }

    // --- Función principal de búsqueda ---
    // Dado un prefijo, navega en el Trie y recolecta todas las continuaciones posibles.
    vector<string> buscarPrefijo(const string& prefijo) const {
        vector<string> resultados;
        int u = 0;
        // 1. Navegar hasta el final del prefijo
        for (char c : prefijo) {
            if (!tabla[u].siguiente.count(c)) return resultados; // No existe
            u = tabla[u].siguiente.at(c);
        }
        // 2. Desde ahí, recolectar todas las terminaciones posibles
        recolectar(u, prefijo, resultados);
        return resultados;
    }
};

// ------------------ AVL -----------------
// Árbol AVL para almacenar los ISBN numéricos y poder listarlos ordenados.
// Provee inserción balanceada y recorrido en orden.
struct AVL {
    struct N {
        long long clave;     // ISBN numérico
        string isbn_str;     // ISBN en formato string (con o sin guiones)
        int altura;          // Altura del nodo (para balance AVL)
        N* izquierda;
        N* derecha;
        N(long long k, const string& s) : clave(k), isbn_str(s), altura(1), izquierda(nullptr), derecha(nullptr) {}
    };
    N* raiz = nullptr;

    // Recorrido InOrder para obtener las claves ordenadas.
    static void recorridoInOrder(N* n, vector<pair<long long, string>>& claves) {
        if (!n) return;
        recorridoInOrder(n->izquierda, claves);
        claves.push_back({ n->clave, n->isbn_str });
        recorridoInOrder(n->derecha, claves);
    }
    vector<pair<long long, string>> obtenerClavesOrdenadas() const {
        vector<pair<long long, string>> claves;
        recorridoInOrder(raiz, claves);
        return claves;
    }

    // Devuelve la altura de un nodo.
    static int Altura(N* n) {
        return n ? n->altura : 0;
    }
    // Recalcula la altura de un nodo en base a sus hijos.
    static void actualizar(N* n) {
        if (n) n->altura = 1 + max(Altura(n->izquierda), Altura(n->derecha));
    }
    // Factor de balance = altura(izquierda) - altura(derecha).
    static int factorBalance(N* n) {
        return n ? Altura(n->izquierda) - Altura(n->derecha) : 0;
    }

    // Rotación simple a la derecha.
    static N* rotacionDerecha(N* y) {
        N* x = y->izquierda;
        N* T = x->derecha;
        x->derecha = y;
        y->izquierda = T;
        actualizar(y);
        actualizar(x);
        return x;
    }
    // Rotación simple a la izquierda.
    static N* rotacionIzquierda(N* x) {
        N* y = x->derecha;
        N* T = y->izquierda;
        y->izquierda = x;
        x->derecha = T;
        actualizar(x);
        actualizar(y);
        return y;
    }

    // Inserta una nueva clave 'k' con su ISBN string 's' en el árbol AVL.
    static N* insercion(N* n, long long k, const string& s) {
        if (!n) return new N(k, s);
        if (k < n->clave) n->izquierda = insercion(n->izquierda, k, s);
        else if (k > n->clave) n->derecha = insercion(n->derecha, k, s);
        else return n; // Clave duplicada, no inserta

        actualizar(n);
        int b = factorBalance(n);

        // Casos de desbalance y sus rotaciones correspondientes:
        if (b > 1 && k < n->izquierda->clave) return rotacionDerecha(n);      // IZQ-IZQ
        if (b < -1 && k > n->derecha->clave) return rotacionIzquierda(n);     // DER-DER
        if (b > 1 && k > n->izquierda->clave) {                                // IZQ-DER
            n->izquierda = rotacionIzquierda(n->izquierda);
            return rotacionDerecha(n);
        }
        if (b < -1 && k < n->derecha->clave) {                                  // DER-IZQ
            n->derecha = rotacionDerecha(n->derecha);
            return rotacionIzquierda(n);
        }
        return n;
    }
    void insertar(long long k, const string& s) { raiz = insercion(raiz, k, s); }
};

// ------------------ Modelos de datos -----------------
// Estructura que representa un libro dentro de la biblioteca.
struct Libro {
    string isbn;
    long long isbn_num = 0;          // ISBN convertido a número para el árbol AVL
    string titulo;
    vector<string> autores;
    string genero;
    string fecha_publi;
    int copias_totales = 0;
    int copias_disponibles = 0;
};

// Estructura que representa un usuario de la biblioteca.
struct Usuario {
    string id_usuario;
    string nombre;
    string correo;
    vector<string> historial_isbn;          // Libros prestados en el historial (por ISBN)
    unordered_set<string> prestamos_activos;// ISBNs actualmente prestados
    int num_prestamos_activos = 0;
    vector<string> historial_titulos;       // Títulos leídos (cuando se devuelven)
};

// Estructura que representa un préstamo individual.
struct Prestamo {
    string id_prestamo;  // ID único del préstamo
    string titulo;       // Título (se puede actualizar si cambia el libro)
    string isbn;         // ISBN del libro prestado
    string id_usuario;   // Usuario que tiene el préstamo
    bool activo = true;  // true mientras el libro no se devuelve
};

// ------------------ Biblioteca -----------------
// Clase principal que maneja toda la lógica de la Biblioteca:
// - Almacenamiento de libros, usuarios y préstamos
// - Persistencia en archivos CSV
// - Búsqueda por Trie y AVL
// - Sistema de lista de espera
// - Sistema de recomendaciones por grafo
// - Historial de acciones para deshacer (undo)
class Biblioteca {
private:
    // --- Persistencia CSV ---
    // Rutas de los archivos CSV (ajustadas a este proyecto en particular)
    const string LIBROS_CSV = "C:/Programacion/C++/Estructura de datos/Proyecto_Final/libros.csv";
    const string USUARIOS_CSV = "C:/Programacion/C++/Estructura de datos/Proyecto_Final/usuarios.csv";
    const string PRESTAMOS_CSV = "C:/Programacion/C++/Estructura de datos/Proyecto_Final/prestamos.csv";
    const string LISTA_ESPERA_CSV = "C:/Programacion/C++/Estructura de datos/Proyecto_Final/lista_espera.csv";
    static const char DELIMITADOR = ',';     // Separador usado en los CSV

    // Estructuras en memoria
    unordered_map<string, Libro> libros;                    // isbn -> Libro
    unordered_map<string, Usuario> usuarios;                // id_usuario -> Usuario
    unordered_map<string, queue<string>> lista_espera;      // isbn -> cola de id_usuarios
    map<string, string> indice;                             // título -> isbn (ordenado alfabéticamente)
    Trie trie;                                              // Trie para autocompletado de búsquedas
    AVL isbn_avl;                                           // AVL para ordenar libros por ISBN numérico

    // Mapa para conectar: Texto de búsqueda (minúsculas) -> Lista de ISBNs
    // Ej: "harry potter" -> [isbn1, isbn2,...] (puede mapear títulos y autores)
    unordered_map<string, vector<string>> mapa_busqueda;

    unordered_map<string, Prestamo> prestamos;              // id_prestamo -> Prestamo

    // Grafo de co-lectura:
    // grafico_libro[A][B] = cuántas veces se han leído juntos A y B por un mismo usuario.
    unordered_map<string, unordered_map<string, int>> grafico_libro;
    // Libros por usuario (no se usa mucho directamente, pero puede usarse para recomendaciones)
    unordered_map<string, unordered_set<string>> libros_usuario;

    // ------------------ SISTEMA DE HISTORIAL / UNDO ------------------
    // Enumeración de tipos de acciones que se pueden registrar para deshacer.
    enum class TipoAccion { 
        AgregarLibro,
        EliminarLibro,
        ModificarLibro,
        AgregarUsuario,
        EliminarUsuario,
        PrestarLibro,
        DevolverLibro,
        PonerenCola
    };
    // Estructura que guarda los datos mínimos de una acción para poder revertirla.
    struct Accion {
        TipoAccion tipo;
        string id;      // ID principal (ej. ISBN o ID Usuario o id_prestamo)
        string usuario; // ID secundario (ej. Usuario en préstamo o en cola)
        string datos;   // Datos extra si se requieren (no muy usado aquí)
    };

    // Pila de historial de acciones (se comporta como LIFO)
    vector<Accion> historial_acciones;

    // Registra una acción en el historial (para ser usada luego por "deshacer")
    void registrar_accion(const Accion& a) { 
        historial_acciones.push_back(a); 
    }
    
    // ----------------------------------------------------------------
    // Manejo de comillas/comas en CSV
    // csv_quote: Si un string contiene el delimitador o comillas,
    // lo envuelve entre comillas dobles y duplica las comillas internas.
    // ----------------------------------------------------------------
    static string csv_quote(const string& s) {
        if (s.find(DELIMITADOR) == string::npos && s.find('"') == string::npos) {
            return s;
        }
        string out = "\"";
        for (char c : s) {
            if (c == '"') out += "\"\"";
            else out.push_back(c);
        }
        out += "\"";
        return out;
    }

    // ----------------------------------------------------------------
    // Parser CSV sencillo pero con soporte para comillas y comillas escapadas
    // parse_csv_line: separa una línea en campos considerando:
    // - Delimitador (coma)
    // - Campos entre comillas
    // - Comillas escapadas con ""
    // ----------------------------------------------------------------
    vector<string> parse_csv_line(const string& line, char delimiter) {
        vector<string> result;
        string field;
        bool in_quotes = false;
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (c == '"') {
                if (in_quotes && i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    i++;
                }
                else {
                    in_quotes = !in_quotes;
                }
            }
            else if (c == delimiter && !in_quotes) {
                result.push_back(field);
                field.clear();
            }
            else {
                field += c;
            }
        }
        result.push_back(field);
        return result;
    }

    // Une los elementos de un vector<string> usando un separador 'sep'.
    static string join(const vector<string>& v, const string& sep) {
        string out;
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) out += sep;
            out += v[i];
        }
        return out;
    }

    // ===================== FUNCIONES DE CSV (LIBROS) =====================

    // Cargar Libros desde el archivo CSV a memoria.
    void cargarLibrosCSV() {
        ifstream file(LIBROS_CSV);
        if (!file.is_open()) {
            cout << "Archivo " << LIBROS_CSV << " no encontrado." << endl;
            return;
        }
        string line;
        getline(file, line); // encabezado
        while (getline(file, line)) {
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            if (campos.size() >= 7) {
                Libro l;
                l.isbn = trim(campos[0]);
                // Se limpia el ISBN para dejar solo dígitos
                l.isbn.erase(remove_if(l.isbn.begin(), l.isbn.end(),
                    [](unsigned char c) { return !isdigit(c); }), l.isbn.end());
                l.titulo = trim(campos[1]);
                // autores separados por '^'
                stringstream ss_autores(campos[2]);
                string autor;
                l.autores.clear();
                while (getline(ss_autores, autor, '^')) {
                    if (!autor.empty()) l.autores.push_back(autor);
                }
                l.genero = trim(campos[3]);
                l.fecha_publi = campos[4];
                try {
                    l.copias_totales = stoi(campos[5]);
                    l.copias_disponibles = stoi(campos[6]);
                }
                catch (const invalid_argument&) {
                    cerr << "Error al convertir copias en linea: " << line << endl;
                    continue;
                }
                // Conversión del ISBN a número para el AVL
                try {
                    l.isbn_num = stoll(l.isbn);
                }
                catch (...) {
                    l.isbn_num = 0;
                }
                // Insertar en los mapas e índices
                libros[l.isbn] = l;
                indice[l.titulo] = l.isbn;
                indexar_termino(l.titulo, l.isbn); // Indexar Titulo para autocompletado
                for (const string& autor : l.autores) {
                    indexar_termino(autor, l.isbn); // Indexar Autores
                }
                if (l.isbn_num != 0) isbn_avl.insertar(l.isbn_num, l.isbn);
            }
        }
        file.close();
        cout << "Libros cargados desde " << LIBROS_CSV << endl;
    }

    // Guardar todos los libros en el archivo CSV.
    void guardarLibrosCSV() {
        ofstream file(LIBROS_CSV);
        if (!file.is_open()) {
            cerr << "Error, no se pudo abrir " << LIBROS_CSV << endl;
            return;
        }
        file << "isbn,titulo,autores,genero,fecha_publi,copias_totales,copias_disponibles\n";
        for (const auto& pair : libros) {
            const Libro& b = pair.second;
            string autores_str = join(b.autores, "^");
            file << csv_quote(b.isbn) << DELIMITADOR
                << csv_quote(b.titulo) << DELIMITADOR
                << csv_quote(autores_str) << DELIMITADOR
                << csv_quote(b.genero) << DELIMITADOR
                << csv_quote(b.fecha_publi) << DELIMITADOR
                << b.copias_totales << DELIMITADOR
                << b.copias_disponibles << "\n";
        }
        file.close();
    }

    // --------------------------------------------------------------------
    // inicializarGrafo:
    // Construye el grafo de co-lectura (libro-libro) a partir de los historiales
    // de ISBN de todos los usuarios.
    // --------------------------------------------------------------------
    void inicializarGrafo() {
        grafico_libro.clear();
        for (const auto& par_u : usuarios) {
            const Usuario& u = par_u.second;
            for (size_t i = 0; i < u.historial_isbn.size(); ++i) {
                for (size_t j = i + 1; j < u.historial_isbn.size(); ++j) {
                    const string& isbn1 = u.historial_isbn[i];
                    const string& isbn2 = u.historial_isbn[j];
                    if (libros.count(isbn1) && libros.count(isbn2)) {
                        grafico_libro[isbn1][isbn2] += 1;
                        grafico_libro[isbn2][isbn1] += 1;
                    }
                }
            }
        }
    }

    // ===================== FUNCIONES DE CSV (USUARIOS) =====================

    // Cargar Usuarios desde CSV.
    void cargarUsuariosCSV() {
        ifstream file(USUARIOS_CSV);
        if (!file.is_open()) {
            cout << "Archivo " << USUARIOS_CSV << " no encontrado. Creando archivo vacío." << endl;
            guardarUsuariosCSV();
            return;
        }
        string line;
        getline(file, line); // encabezado
        while (getline(file, line)) {
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            // Esperamos 6 campos: id,nombre,correo,prestamos_activos,historial_isbn,historial_titulos
            if (campos.size() >= 6) {
                Usuario u;
                u.id_usuario = campos[0];
                u.nombre = campos[1];
                u.correo = campos[2];
                // prestamos_activos deserializar (^)
                u.prestamos_activos.clear();
                stringstream ss_pa(campos[3]);
                string isbn_pa;
                while (getline(ss_pa, isbn_pa, '^')) {
                    if (!isbn_pa.empty()) u.prestamos_activos.insert(isbn_pa);
                }
                // historial_isbn
                u.historial_isbn.clear();
                stringstream ss_h(campos[4]);
                string isbn_h;
                while (getline(ss_h, isbn_h, '^')) {
                    if (!isbn_h.empty()) u.historial_isbn.push_back(isbn_h);
                }
                // historial_titulos (campo 5)
                u.historial_titulos.clear();
                stringstream ss_t(campos[5]);
                string tit;
                while (getline(ss_t, tit, '^')) {
                    if (!tit.empty()) u.historial_titulos.push_back(tit);
                }
                usuarios[u.id_usuario] = u;
            }
        }
        file.close();
        cout << "Usuarios cargados desde " << USUARIOS_CSV << endl;
    }

    // Guardar Usuarios en CSV.
    void guardarUsuariosCSV() {
        ofstream file(USUARIOS_CSV);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << USUARIOS_CSV << " para escritura." << endl;
            return;
        }
        file << "id_usuario,nombre,correo,prestamos_activos,historial_isbn,historial_titulos_lectura\n";
        for (const auto& pair : usuarios) {
            const Usuario& u = pair.second;
            // Convertir prestamos_activos (set) a vector para usar join.
            string prestamos_str = join(vector<string>(u.prestamos_activos.begin(), u.prestamos_activos.end()), "^");
            string historial_isbn_str = join(u.historial_isbn, "^");
            // crear cadena de titulos
            vector<string> historial_titulos_vec;
            for (const string& isbn : u.historial_isbn) {
                if (libros.count(isbn)) historial_titulos_vec.push_back(libros.at(isbn).titulo);
                else historial_titulos_vec.push_back("ISBN_NO_ENCONTRADO");
            }
            // si historial_titulos explícito existe (guardado antes), lo usamos en preferencia
            if (!u.historial_titulos.empty()) {
                historial_titulos_vec = u.historial_titulos;
            }
            string historial_titulos_str = join(historial_titulos_vec, "^");
            file << csv_quote(u.id_usuario) << DELIMITADOR
                << csv_quote(u.nombre) << DELIMITADOR
                << csv_quote(u.correo) << DELIMITADOR
                << csv_quote(prestamos_str) << DELIMITADOR
                << csv_quote(historial_isbn_str) << DELIMITADOR
                << csv_quote(historial_titulos_str) << "\n";
        }
        file.close();
    }

    // ===================== FUNCIONES DE CSV (PRESTAMOS) =====================

    // Cargar préstamos desde CSV.
    void cargarPrestamosCSV() {
        ifstream file(PRESTAMOS_CSV);
        if (!file.is_open()) {
            cout << "Archivo " << PRESTAMOS_CSV << " no encontrado. Creando archivo vacío." << endl;
            guardarPrestamosCSV();
            return;
        }
        string line;
        getline(file, line);
        while (getline(file, line)) {
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            if (campos.size() >= 6) {
                Prestamo p;
                p.id_prestamo = campos[0];
                p.isbn = campos[1];
                p.id_usuario = campos[2];
                // campos[3] nombre_usuario (no se guarda en Prestamo)
                // campos[4] titulo (no se guarda directamente aquí)
                p.activo = (campos[5] == "true");
                prestamos[p.id_prestamo] = p;
            }
        }
        file.close();
        cout << "Prestamos cargados desde " << PRESTAMOS_CSV << endl;
    }

    // Guardar préstamos en CSV.
    void guardarPrestamosCSV() {
        ofstream file(PRESTAMOS_CSV);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << PRESTAMOS_CSV << " para escritura." << endl;
            return;
        }
        file << "id_prestamo,isbn,id_usuario,nombre_usuario,titulo,activo\n";
        for (const auto& pair : prestamos) {
            const Prestamo& p = pair.second;
            string nombre_usuario = usuarios.count(p.id_usuario) ? usuarios.at(p.id_usuario).nombre : "NO ENCONTRADO";
            string titulo_libro = libros.count(p.isbn) ? libros.at(p.isbn).titulo : "NO ENCONTRADO";
            string activo_str = p.activo ? "true" : "false";
            file << csv_quote(p.id_prestamo) << DELIMITADOR
                << csv_quote(p.isbn) << DELIMITADOR
                << csv_quote(p.id_usuario) << DELIMITADOR
                << csv_quote(nombre_usuario) << DELIMITADOR
                << csv_quote(titulo_libro) << DELIMITADOR
                << activo_str << "\n";
        }
        file.close();
    }

    // ===================== FUNCIONES DE CSV (LISTA DE ESPERA) =====================

    // Cargar lista de espera desde CSV.
    void cargarListaEsperaCSV() {
        ifstream file(LISTA_ESPERA_CSV);
        if (!file.is_open()) return; // Si no existe, no pasa nada
        
        string line;
        getline(file, line); // saltar encabezado
        while (getline(file, line)) {
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            if (campos.size() >= 2) {
                string isbn = campos[0];
                string cadena_usuarios = campos[1];
                queue<string> q;
                stringstream ss(cadena_usuarios);
                string u;
                while (getline(ss, u, '^')) { // Separados por ^
                    if (!u.empty()) q.push(u);
                }
                if (!q.empty()) lista_espera[isbn] = std::move(q);
            }
        }
        file.close();
        cout << "Lista de espera cargada desde " << LISTA_ESPERA_CSV << endl;
    }

    // Guardar lista de espera en CSV.
    void guardarListaEsperaCSV() {
        ofstream file(LISTA_ESPERA_CSV);
        if (!file.is_open()) return;
        
        file << "isbn,cola_usuarios\n";
        for (auto& p : lista_espera) {
            const string& isbn = p.first;
            queue<string> q = p.second; // Copia temporal para recorrer
            vector<string> usuarios_vec;
            while (!q.empty()) {
                usuarios_vec.push_back(q.front());
                q.pop();
            }
            string cadena = join(usuarios_vec, "^");
            // Solo guardamos si hay gente en espera
            if (!cadena.empty()) {
                file << csv_quote(isbn) << DELIMITADOR << csv_quote(cadena) << "\n";
            }
        }
        file.close();
    }

    // ===================== HELPERS INTERNOS =====================

    // Genera un ID de préstamo con prefijo 'P' y 8 dígitos aleatorios.
    string generar_id_prestamo() {
    return "P" + generar_id_aleatorio(8);
    }

    // Helper privado para indexar texto en Trie y en el mapa de búsqueda.
    // 'texto' se normaliza a minúsculas, se inserta en el Trie y se asocia al ISBN.
    void indexar_termino(string texto, const string& isbn) {
    if (texto.empty()) return;
    
    // Convertir a minúsculas para estandarizar
    for (auto& c : texto) c = tolower(c);
    
    // Insertar en Trie
    trie.insertar(texto);
    
    // Mapear texto -> ISBN
    mapa_busqueda[texto].push_back(isbn);
    }

    // --------------------------------------------------------------------
    // deshacer_accion:
    // Lógica interna para revertir una acción registrada en el historial.
    // Nota: No todas las acciones están completamente implementadas para undo.
    // --------------------------------------------------------------------
    void deshacer_accion(const Accion& a) { 
        switch (a.tipo) {
        case TipoAccion::AgregarLibro:
            // Si la acción fue agregar, deshacer es ELIMINAR el libro
            if (libros.count(a.id)) {
                // Borrado manual rápido para no usar quitarLibros y evitar bucles
                libros.erase(a.id);
                // Borrar del índice
                for (auto it = indice.begin(); it != indice.end(); ) {
                    if (it->second == a.id) it = indice.erase(it);
                    else ++it;
                }
                cout << "Acción revertida: Libro " << a.id << " eliminado." << endl;
                
                // Actualizar CSVs
                guardarLibrosCSV();
            }
            break;

        case TipoAccion::EliminarLibro:
            cout << "Deshacer 'EliminarLibro' no implementado (requiere guardar todos los datos del libro borrado)." << endl;
            break;

        case TipoAccion::ModificarLibro:
            cout << "Deshacer 'ModificarLibro' no implementado." << endl;
            break;

        case TipoAccion::AgregarUsuario:
            // Si la acción fue agregar, deshacer es ELIMINAR el usuario
            if (usuarios.count(a.id)) {
                usuarios.erase(a.id);
                cout << "Acción revertida: Usuario " << a.id << " eliminado." << endl;
                
                // Actualizar CSVs
                guardarUsuariosCSV();
            }
            break;

        case TipoAccion::EliminarUsuario:
            cout << "Deshacer 'EliminarUsuario' no implementado." << endl;
            break;

        case TipoAccion::PrestarLibro:
            // Si se prestó, deshacer es CANCELAR el préstamo (borrarlo y devolver copia)
            if (prestamos.count(a.id)) {
                Prestamo P = prestamos[a.id];
                
                // 1. Borrar el préstamo
                prestamos.erase(a.id);
                
                // 2. Recuperar la copia del libro
                if (libros.count(P.isbn)) {
                    libros[P.isbn].copias_disponibles += 1;
                }
                
                // 3. Quitar del historial activo del usuario
                if (usuarios.count(P.id_usuario)) {
                    usuarios[P.id_usuario].prestamos_activos.erase(P.isbn);
                    if (usuarios[P.id_usuario].num_prestamos_activos > 0)
                        usuarios[P.id_usuario].num_prestamos_activos--;
                    
                    // Nota: Quitarlo del historial_isbn es complejo sin saber si lo leyó antes, 
                    // así que solo lo quitamos de activos.
                }
                cout << "Acción revertida: Préstamo " << a.id << " cancelado." << endl;

                // Actualizar TODOS los CSVs
                guardarLibrosCSV();
                guardarUsuariosCSV();
                guardarPrestamosCSV();
            }
            break;

        case TipoAccion::DevolverLibro:
            // Revertir una devolución implicaría recrear el préstamo y ajustar copias/lista_espera
            // Esto no está implementado en detalle en este proyecto.
            cout << "Deshacer 'DevolverLibro' no implementado completamente." << endl;
            break;

            case TipoAccion::PonerenCola:
            // Deshacer poner en cola = sacar al usuario de la cola del libro
            if (lista_espera.count(a.id)) {
                // Hay que reconstruir la cola quitando al usuario específico
                // a.id es el ISBN, a.usuario es el ID del usuario
                queue<string> original = lista_espera[a.id];
                queue<string> nueva;
                bool eliminado = false;
                
                while (!original.empty()) {
                    string u = original.front();
                    original.pop();
                    // Eliminar solo la primera ocurrencia de este usuario
                    if (!eliminado && u == a.usuario) {
                        eliminado = true; // Lo saltamos (lo borramos)
                    } else {
                        nueva.push(u);
                    }
                }
                lista_espera[a.id] = nueva;
                cout << "Acción revertida: Usuario " << a.usuario << " sacado de la cola de espera." << endl;
                guardarListaEsperaCSV();
            }
            break;

        default:
            cout << "Acción desconocida." << endl;
            break;
        }
    }

public:
    // Constructor de la clase Biblioteca:
    // Carga todos los datos desde CSV al iniciar el programa y prepara el grafo.
    Biblioteca() {
        cargarLibrosCSV();
        cargarUsuariosCSV();
        cargarPrestamosCSV();
        cargarListaEsperaCSV();
        inicializarGrafo();
    }

    // =====================================================================
    //                           GESTION DE LIBROS
    // =====================================================================

    // Agrega un libro a la estructura 'libros' y actualiza índices y CSV.
    bool agregarLibro(const Libro& libro) {
        if (libros.count(libro.isbn)) return false;
        libros[libro.isbn] = libro;
        indice[libro.titulo] = libro.isbn;
        indexar_termino(libro.titulo, libro.isbn); // Indexar Titulo
        for (const string& autor : libro.autores) {
            indexar_termino(autor, libro.isbn); // Indexar Autores
        }
        if (libro.isbn_num != 0) isbn_avl.insertar(libro.isbn_num, libro.isbn);
        registrar_accion({ TipoAccion::AgregarLibro, libro.isbn, "", "" });
        guardarLibrosCSV();
        return true;
    }

    // Elimina por completo un libro (ISBN) de la biblioteca, limpiando:
    // - Prestamos asociados
    // - Referencias en usuarios
    // - Índices e historial
    bool quitarLibros(const string& isbn) {
    if (!libros.count(isbn)) return false;

    // 1. Limpiar PRÉSTAMOS asociados a este libro
    // Recolectamos IDs de prestamos a borrar para no romper el iterador
    vector<string> prestamos_a_borrar;
    for (auto& par : prestamos) {
        if (par.second.isbn == isbn) {
            prestamos_a_borrar.push_back(par.first);
        }
    }
    for (const string& pid : prestamos_a_borrar) {
        prestamos.erase(pid);
    }

    // 2. Limpiar USUARIOS (Historiales y Préstamos Activos)
    for (auto& par_usuario : usuarios) {
        Usuario& u = par_usuario.second;

        // A. Si el usuario lo tiene prestado actualmente, lo quitamos
        if (u.prestamos_activos.count(isbn)) {
            u.prestamos_activos.erase(isbn);
            u.num_prestamos_activos = max(0, u.num_prestamos_activos - 1);
        }

        // B. Limpiar del historial de ISBNs
        // (remove_if mueve los elementos a borrar al final, erase los elimina)
        auto it_isbn = remove(u.historial_isbn.begin(), u.historial_isbn.end(), isbn);
        if (it_isbn != u.historial_isbn.end()) {
            u.historial_isbn.erase(it_isbn, u.historial_isbn.end());
        }

        // C. Limpiar del historial de Títulos (Esto es más complejo porque solo tenemos el titulo string)
        // Nota: Como borramos el libro, obtenemos su titulo antes de borrarlo del mapa 'libros'
        string titulo_a_borrar = libros[isbn].titulo;
        auto it_titulo = remove(u.historial_titulos.begin(), u.historial_titulos.end(), titulo_a_borrar);
        if (it_titulo != u.historial_titulos.end()) {
            u.historial_titulos.erase(it_titulo, u.historial_titulos.end());
        }
    }

    // 3. Eliminar del índice auxiliar (Titulo -> ISBN)
    for (auto it = indice.begin(); it != indice.end(); ) {
        if (it->second == isbn) it = indice.erase(it);
        else ++it;
    }

    // 4. Eliminar el libro del mapa principal
    libros.erase(isbn);

    // 5. GUARDAR TODO (Para mantener sincronía)
    guardarLibrosCSV();
    guardarPrestamosCSV(); // Actualizamos prestamos
    guardarUsuariosCSV();  // Actualizamos usuarios
    
    return true;
}

    // Modifica completamente la información de un libro existente (mismo ISBN).
    // Actualiza también los títulos en usuarios y préstamos si el título cambia.
    bool modificarLibro(const string& isbn, const Libro& nuevo) {
    if (!libros.count(isbn)) return false;
    
    Libro viejo = libros[isbn];
    string titulo_anterior = viejo.titulo;

    // Actualizar mapa principal
    libros[isbn] = nuevo;

    // Actualizar índice
    for (auto it = indice.begin(); it != indice.end(); ++it) {
        if (it->second == isbn) { indice.erase(it); break; }
    }
    indice[nuevo.titulo] = isbn;
    indexar_termino(nuevo.titulo, nuevo.isbn); // Indexar Titulo
    for (const string& autor : nuevo.autores) {
        indexar_termino(autor, nuevo.isbn); // Indexar Autores
    }

    // --- CASCADA: Actualizar título en Usuarios y Préstamos ---
    if (titulo_anterior != nuevo.titulo) {
        // 1. Actualizar Préstamos
        for (auto& par : prestamos) {
            if (par.second.isbn == isbn) {
                par.second.titulo = nuevo.titulo;
            }
        }

        // 2. Actualizar Historial de Usuarios (Aquí sí es texto plano)
        for (auto& par_u : usuarios) {
            Usuario& u = par_u.second;
            for (string& t : u.historial_titulos) {
                if (t == titulo_anterior) {
                    t = nuevo.titulo;
                }
            }
        }
    }
    // ----------------------------------------------------------

    guardarLibrosCSV();
    guardarUsuariosCSV(); // Actualizamos por si cambiaron los historiales de titulos
    guardarPrestamosCSV(); // Actualizamos por consistencia

    return true;
}

    // Imprime en pantalla la información de un libro por ISBN.
    void mostrar_libro(const string& isbn) const {
        if (!libros.count(isbn)) { cout << "No existe libro con ISBN " << isbn << endl; return; }
        const Libro& b = libros.at(isbn);
        cout << "ISBN: " << b.isbn << " - Titulo: " << b.titulo << " - Genero: " << b.genero << " - Autores: ";
        for (size_t i = 0; i < b.autores.size(); ++i) {
            if (i) cout << ", ";
            cout << b.autores[i];
        }
        cout << " - Fecha: " << b.fecha_publi << " - Copias: " << b.copias_disponibles << "/" << b.copias_totales << endl;
    }

    // Muestra los libros ordenados por ISBN numérico usando el árbol AVL.
    void mostrar_libros_por_isbn_num_ordenado() const {
        cout << "--- Libros ordenados por ISBN numerico (via AVL) ---" << endl;
        vector<pair<long long, string>> isbns_ordenados = isbn_avl.obtenerClavesOrdenadas();
        if (isbns_ordenados.empty()) { cout << "No hay libros con ISBN numerico valido." << endl; return; }
        for (const auto& p : isbns_ordenados) {
            cout << "ISBN Num: " << p.first << endl;
            mostrar_libro(p.second);
        }
    }

    // Devuelve un puntero constante al Libro si existe el ISBN, o nullptr si no.
    const Libro* obtenerLibroPorISBN(const string& isbn) const {
        auto it = libros.find(isbn);
        if (it != libros.end()) return &(it->second);
        return nullptr;
    }

    // Muestra todos los libros que coincidan (parcialmente) con un género dado.
    // Se hace una búsqueda case-insensitive y sin caracteres especiales.
    void mostrar_libros_por_genero(const string& genero) const {
        string genero_busqueda = genero;
        transform(genero_busqueda.begin(), genero_busqueda.end(), genero_busqueda.begin(),
            [](unsigned char c) { return tolower(c); });
        genero_busqueda.erase(remove_if(genero_busqueda.begin(), genero_busqueda.end(),
            [](unsigned char c) { return !isalnum(c); }), genero_busqueda.end());

        cout << "--- Libros del género: " << genero << " ---" << endl;
        bool encontrado = false;
        for (const auto& par : libros) {
            const Libro& l = par.second;
            string l_gen = l.genero;
            transform(l_gen.begin(), l_gen.end(), l_gen.begin(),
                [](unsigned char c) { return tolower(c); });
            l_gen.erase(remove_if(l_gen.begin(), l_gen.end(),
                [](unsigned char c) { return !isalnum(c); }), l_gen.end());
            if (l_gen.find(genero_busqueda) != string::npos) {
                mostrar_libro(l.isbn);
                encontrado = true;
            }
        }
        if (!encontrado) cout << "No se encontraron libros del género " << genero << endl;
    }

    // =====================================================================
    //                           GESTION DE USUARIOS
    // =====================================================================

    // Agregar un usuario nuevo. El ID debe ser único.
    bool agregarUsuario(const Usuario& u) {
        if (usuarios.count(u.id_usuario)) return false;
        usuarios[u.id_usuario] = u;
        registrar_accion({ TipoAccion::AgregarUsuario, u.id_usuario, "", "" });
        guardarUsuariosCSV();
        return true;
    }

    // Eliminar un usuario completamente:
    // - Devuelve copias de los libros que tenga activos
    // - Elimina préstamos asociados a ese usuario
    // - Lo quita del mapa de usuarios
    bool eliminarUsuario(const string& uid) {
    if (!usuarios.count(uid)) return false;

    Usuario& u = usuarios[uid];

    // 1. Devolver libros que el usuario tenga activos (Recuperar stock)
    for (const string& isbn : u.prestamos_activos) {
        if (libros.count(isbn)) {
            libros[isbn].copias_disponibles++;
        }
    }

    // 2. Eliminar PRÉSTAMOS asociados a este usuario en el archivo de prestamos
    vector<string> prestamos_a_borrar;
    for (auto& par : prestamos) {
        if (par.second.id_usuario == uid) {
            prestamos_a_borrar.push_back(par.first);
        }
    }
    for (const string& pid : prestamos_a_borrar) {
        prestamos.erase(pid);
    }

    // 3. Eliminar usuario del mapa principal
    usuarios.erase(uid);

    // 4. GUARDAR TODO
    guardarUsuariosCSV();
    guardarLibrosCSV();    // Necesario porque cambiamos copias_disponibles
    guardarPrestamosCSV(); // Necesario porque borramos registros de prestamos
    
    return true;
}

    // =====================================================================
    //                       PRESTAMOS / DEVOLUCIONES
    // =====================================================================

    // prestamoLibro:
    // Intenta prestar un libro a un usuario.
    // Devuelve:
    //  1  -> Préstamo exitoso
    //  2  -> No hay copias, usuario agregado a la lista de espera
    //  0  -> ISBN no existe
    // -1  -> Usuario no existe
    int prestamoLibro(const string& id_usuario, const string& isbn) {
        if (!usuarios.count(id_usuario)) return -1; // Usuario no existe
        if (!libros.count(isbn)) return 0;          // ISBN no existe

        Libro& l = libros[isbn];

        // CASO 1: Hay copias disponibles -> Prestar
        if (l.copias_disponibles > 0) {
            l.copias_disponibles -= 1;
            usuarios[id_usuario].prestamos_activos.insert(isbn);
            usuarios[id_usuario].historial_isbn.push_back(isbn);
            usuarios[id_usuario].num_prestamos_activos++;

            // Actualizar grafo de co-lectura con el nuevo libro del usuario
            for (const string& otro : usuarios[id_usuario].historial_isbn) {
                if (otro == isbn) continue;
                grafico_libro[isbn][otro] += 1;
                grafico_libro[otro][isbn] += 1;
            }

            Prestamo P;
            P.id_prestamo = generar_id_prestamo();
            P.isbn = isbn;
            P.id_usuario = id_usuario;
            P.activo = true;
            prestamos[P.id_prestamo] = P;

            registrar_accion({ TipoAccion::PrestarLibro, P.id_prestamo, id_usuario, isbn });

            guardarLibrosCSV();
            guardarPrestamosCSV();
            guardarUsuariosCSV();
            return 1; // ÉXITO
        }
        
        // CASO 2: No hay copias -> Cola de espera
        else {
            lista_espera[isbn].push(id_usuario);
            
            registrar_accion({ TipoAccion::PonerenCola, isbn, id_usuario, "" });
            
            guardarListaEsperaCSV(); // Actualizamos solo la cola
            return 2; // EN COLA
        }
    }

    // devolver_libro:
    // Procesa la devolución de un libro:
    // - Marca el préstamo como inactivo
    // - Actualiza el historial de títulos del usuario
    // - Si hay lista de espera, asigna automáticamente el libro al siguiente usuario
    // - Si no hay lista de espera, aumenta copias_disponibles
    bool devolver_libro(const string& id_usuario, const string& isbn) {
        if (!usuarios.count(id_usuario) || !libros.count(isbn)) return false;

        // 1. Buscar y desactivar el préstamo actual
        string pid_actual = "";
        for (auto& p : prestamos) {
            if (p.second.isbn == isbn && p.second.id_usuario == id_usuario && p.second.activo) {
                pid_actual = p.first;
                break;
            }
        }
        if (pid_actual.empty()) return false;

        prestamos[pid_actual].activo = false;
        usuarios[id_usuario].prestamos_activos.erase(isbn);
        if (usuarios[id_usuario].num_prestamos_activos > 0) usuarios[id_usuario].num_prestamos_activos--;
        
        // Historial de títulos (lectura finalizada)
        usuarios[id_usuario].historial_titulos.push_back(libros[isbn].titulo);

        registrar_accion({ TipoAccion::DevolverLibro, pid_actual, id_usuario, isbn });

        // 2. VERIFICAR COLA DE ESPERA
        if (!lista_espera[isbn].empty()) {
            // A. Sacar al siguiente usuario
            string siguiente_usuario = lista_espera[isbn].front();
            lista_espera[isbn].pop();

            // B. Crear préstamo automático para él (Sin tocar copias_disponibles porque 1 entra, 1 sale)
            if (usuarios.count(siguiente_usuario)) {
                Usuario& u_next = usuarios[siguiente_usuario];
                u_next.prestamos_activos.insert(isbn);
                u_next.historial_isbn.push_back(isbn);
                u_next.num_prestamos_activos++;
                
                // Actualizar grafo para el nuevo usuario
                for (const string& otro : u_next.historial_isbn) {
                    if (otro == isbn) continue;
                    grafico_libro[isbn][otro] += 1;
                    grafico_libro[otro][isbn] += 1;
                }

                Prestamo P;
                P.id_prestamo = generar_id_prestamo();
                P.isbn = isbn;
                P.id_usuario = siguiente_usuario;
                P.activo = true;
                prestamos[P.id_prestamo] = P;
                
                // Nota: No registramos acción de "Prestar" aquí para simplificar el Undo del devolver.
            }
            cout << ">>> AVISO: El libro ha sido asignado automáticamente a " << siguiente_usuario << " de la lista de espera." << endl;
        } 
        else {
            // Si nadie espera, el libro vuelve al estante
            libros[isbn].copias_disponibles += 1;
        }

        // 3. ACTUALIZAR TODOS LOS CSV
        guardarLibrosCSV();
        guardarPrestamosCSV();
        guardarUsuariosCSV();
        guardarListaEsperaCSV();

        return true;
    }

    // =====================================================================
    //                           RECOMENDACIONES
    // =====================================================================

    // recomendar_para_usuario:
    // Devuelve una lista de hasta K recomendaciones (ISBN, score) para un usuario.
    // Usa el grafo de co-lectura: libros que otros han leído junto con los libros del usuario.
    // Puede filtrar por género si 'genero_preferido' no está vacío.
    vector<pair<string, int>> recomendar_para_usuario(const string& id_usuario, const string& genero_preferido, int K = 10) {
        vector<pair<string, int>> res;
        if (!usuarios.count(id_usuario)) return res;
        unordered_map<string, int> scores;
        unordered_set<string> seen(usuarios[id_usuario].historial_isbn.begin(), usuarios[id_usuario].historial_isbn.end());
        string genero_low = genero_preferido;
        for (char& c : genero_low) c = tolower(c);
        for (const string& b : usuarios[id_usuario].historial_isbn) {
            if (grafico_libro.count(b)) {
                for (auto& p : grafico_libro[b]) {
                    const string& isbn_recomendado = p.first;
                    if (seen.count(isbn_recomendado)) continue;
                    if (!genero_preferido.empty() && libros.count(isbn_recomendado)) {
                        string libro_genero_low = libros[isbn_recomendado].genero;
                        for (char& c : libro_genero_low) c = tolower(c);
                        if (libro_genero_low.find(genero_low) == string::npos) continue;
                    }
                    scores[isbn_recomendado] += p.second;
                }
            }
        }
        for (auto& p : scores) res.push_back({ p.first, p.second });
        sort(res.begin(), res.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
            });
        if ((int)res.size() > K) res.resize(K);
        return res;
    }

// Esta función busca por Título Y Autor usando el Trie
// autocompletado_general:
// 1) Normaliza el prefijo a minúsculas
// 2) Usa el Trie para obtener todas las coincidencias completas
// 3) Traduce esas claves a títulos legibles usando 'mapa_busqueda' y 'libros'
vector<string> autocompletado_general(const string& prefijo, int K = 10) {
    vector<string> sugerencias_finales;
    if (prefijo.empty()) return sugerencias_finales;

    // 1. Normalizar prefijo a minúsculas
    string p_low = prefijo;
    for (auto& c : p_low) c = tolower(c);

    // 2. Usar el Trie para obtener las cadenas completas (ej. "harry po" -> "harry potter")
    vector<string> coincidencias_trie = trie.buscarPrefijo(p_low);

    // 3. Recuperar los títulos reales formateados usando el mapa_busqueda
    int contador = 0;
    // Usamos un set para evitar duplicados si título y autor se parecen
    unordered_set<string> isbns_vistos; 

    for (const string& llave : coincidencias_trie) {
        if (mapa_busqueda.count(llave)) {
            for (const string& isbn : mapa_busqueda[llave]) {
                if (isbns_vistos.count(isbn)) continue;
                
                // Formateamos la salida bonita: "Título (Autor)"
                if (libros.count(isbn)) {
                    string display = libros[isbn].titulo + " (Autor: " + join(libros[isbn].autores, ", ") + ")";
                    sugerencias_finales.push_back(display);
                    isbns_vistos.insert(isbn);
                    contador++;
                }
                if (contador >= K) return sugerencias_finales;
            }
        }
    }
    return sugerencias_finales;
}

    // =====================================================================
    //                     MOSTRAR USUARIO / TITULOS
    // =====================================================================

    // Muestra la información principal de un usuario:
    // - ID, nombre, correo
    // - Libros prestados (ISBN)
    // - Historial de lecturas (según historial_isbn)
    void mostrar_usuario(const string& uid) {
        if (!usuarios.count(uid)) { cout << "No existe usuario " << uid << endl; return; }
        const Usuario& u = usuarios.at(uid);
        cout << "Usuario: " << u.id_usuario << " - " << u.nombre << " (" << u.correo << ") Prestados (ISBNs activos): ";
        for (const string& isbn : u.prestamos_activos) cout << isbn << " ";
        cout << "\nHistorial de lecturas (Títulos): ";
        for (const string& isbn : u.historial_isbn) {
            if (libros.count(isbn)) cout << "[" << libros.at(isbn).titulo << "] ";
            else cout << "[ISBN: " << isbn << " NO ENCONTRADO] ";
        }
        cout << endl;
    }

    // Imprime todos los títulos ordenados alfabéticamente usando 'indice' (map).
    void mostrar_titulos_ordenados() {
        cout << "Titulos ordenados: " << endl;
        for (auto& p : indice) cout << p.first << " (" << p.second << ")" << endl;
    }

    // =====================================================================
    //                      DESHACER ÚLTIMA OPERACIÓN
    // =====================================================================

    // deshacerUltimaOperacion:
    // Toma la última acción registrada en 'historial_acciones' y llama a 'deshacer_accion'.
    void deshacerUltimaOperacion() {
        if (historial_acciones.empty()) {
            cout << "No hay acciones para deshacer." << endl;
            return;
        }
        
        // Tomamos la última acción (LIFO)
        Accion ultima = historial_acciones.back();
        historial_acciones.pop_back(); // La sacamos de la pila
        
        // Llamamos a la función interna que ya tiene los couts y la lógica
        deshacer_accion(ultima); 
    }
};

int main() {
    // Usar locale del sistema para no perder acentos en consola Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    Biblioteca B;

    // Menú principal del sistema de biblioteca
    string menu = "\nBienvenido a la Biblioteca Inteligente \n------------------------------------- \n1. Agregar libro \n2. Eliminar libro \n3. Modificar libro \n4. Agregar usuario \n5. Eliminar usuario \n6. Prestar libro \n7. Devolver libro \n8. Buscar titulo (Autocompletar) \n9. Listar libros (Ordenado por título) \n10. Recomendar libros \n11. Ver libro \n12. Ver usuario \n13. Listar libros por ISBN Numerico (AVL) \n14. Deshacer la última acción \n15. Buscar por género \n16. Salir\n";

    int opcion;
    while (true) {
        cout << menu;
        cout << "\nSeleccione una opcion: ";
        if (!(cin >> opcion)) {
            break;
        }
        cin.ignore();
        if (opcion == 16) {
            break;
        }
        switch (opcion) {
            case 1: {
                // ---------------- OPCIÓN 1: Agregar libro ----------------
                Libro nb;
                cout << "ISBN (Presione ENTER para generar uno automático 978...): ";
                // Usamos getline para detectar si dio Enter vacío
                string entrada_isbn;
                getline(cin, entrada_isbn);

                // Limpiar espacios por si acaso
                entrada_isbn = trim(entrada_isbn);

                if (entrada_isbn.empty()) {
                    // TRUCO: "978" + 10 dígitos aleatorios = ISBN-13 válido (en forma básica)
                    nb.isbn = "978" + generar_id_aleatorio(10);
                    cout << ">>> ISBN generado automáticamente: " << nb.isbn << endl;
                } else {
                    nb.isbn = entrada_isbn;
                }

                //Convertir el string a número para el AVL
                string solo_numeros = nb.isbn;
                // Eliminamos guiones o letras para la conversión numérica
                solo_numeros.erase(remove_if(solo_numeros.begin(), solo_numeros.end(), 
                    [](unsigned char c) { return !isdigit(c); }), solo_numeros.end());
                try {
                    if (!solo_numeros.empty()) nb.isbn_num = stoll(solo_numeros);
                    else nb.isbn_num = 0;
                } catch (...) {
                    nb.isbn_num = 0;
                }
                cin.ignore();

                cout << "Titulo: ";
                getline(cin, nb.titulo);
                
                // (Opcional) Forzar primera letra mayúscula para ayudar al ordenamiento del Case 9
                if (!nb.titulo.empty()) nb.titulo[0] = toupper(nb.titulo[0]);

                cout << "Autor(es) (separados por ;): ";
                string a;
                getline(cin, a);
                nb.autores.clear();
                {
                    stringstream ss_a(a);
                    string t;
                    while (getline(ss_a, t, ';')) {
                        if (!t.empty()) {
                            nb.autores.push_back(t);
                        }
                    }
                }
                cout << "Genero: ";
                getline(cin, nb.genero);

                int anio, mes, dia;
                
                // 1. Pedir Año
                cout << "  Año de publicación: ";
                while (!(cin >> anio)) { // Validación básica de número
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "  Por favor ingrese un número válido para el año: ";
                }

                // 2. Pedir Mes
                cout << "  Mes de publicación: ";
                while (!(cin >> mes) || mes < 1 || mes > 12) {
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "  Mes inválido. Ingrese entre 1 y 12: ";
                }

                // 3. Pedir Día
                cout << "  Día de publicación: ";
                while (!(cin >> dia) || dia < 1 || dia > 31) {
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "  Día inválido. Ingrese entre 1 y 31: ";
                }

                // 4. Formatear a String YYYY-MM-DD
                // El operador ternario (condicion ? si : no) agrega un '0' si el número es menor a 10
                string s_mes = (mes < 10 ? "0" : "") + to_string(mes);
                string s_dia = (dia < 10 ? "0" : "") + to_string(dia);
                
                nb.fecha_publi = to_string(anio) + "-" + s_mes + "-" + s_dia;

                cout << "Copias totales: ";
                cin >> nb.copias_totales;
                nb.copias_disponibles = nb.copias_totales;
                
                if (B.agregarLibro(nb)) {
                    cout << "Libro agregado y guardado en CSV." << endl;
                }
                else {
                    cout << "ISBN ya existe." << endl;
                }
            } break;
            case 2: {
                // ---------------- OPCIÓN 2: Eliminar libro ----------------
                string isbn;
                cout << "ISBN: ";
                cin >> isbn;
                if (B.quitarLibros(isbn)) {
                    cout << "Libro removido y CSV actualizado." << endl;
                }
                else {
                    cout << "No existe." << endl;
                }
            } break;
            case 3: {
                // ---------------- OPCIÓN 3: Modificar libro ----------------
                string isbn;
                cout << "ISBN a modificar: ";
                cin >> isbn;
                cin.ignore();
                if (isbn.empty()){
                    cout << "Se necesita un ISBN para modificar" << endl;
                    break;
                }
                Libro nb;
                nb.isbn = isbn;
                cout << "Nuevo titulo: ";
                getline(cin, nb.titulo);
                cout << "Autor(es) (separados con ;): ";
                string a;
                getline(cin, a);
                nb.autores.clear();
                {
                    stringstream ss_a(a);
                    string t;
                    while (getline(ss_a, t, ';')) {
                        if (!t.empty()) {
                            nb.autores.push_back(t);
                        }
                    }
                }
                cout << "Nuevo Genero: ";
                getline(cin, nb.genero);

                int anio, mes, dia;
                
                cout << "  Año de publicación: ";
                while (!(cin >> anio)) { cin.clear(); cin.ignore(1000, '\n'); cout << "Año numérico: "; }
                
                cout << "  Mes de publicación (1-12): ";
                while (!(cin >> mes) || mes < 1 || mes > 12) { cin.clear(); cin.ignore(1000, '\n'); cout << "Mes (1-12): "; }
                
                cout << "  Día de publicación (1-31): ";
                while (!(cin >> dia) || dia < 1 || dia > 31) { cin.clear(); cin.ignore(1000, '\n'); cout << "Día (1-31): "; }

                string s_mes = (mes < 10 ? "0" : "") + to_string(mes);
                string s_dia = (dia < 10 ? "0" : "") + to_string(dia);
                
                nb.fecha_publi = to_string(anio) + "-" + s_mes + "-" + s_dia;
                cout << "Copias totales: ";
                cin >> nb.copias_totales;
                nb.copias_disponibles = max(0, nb.copias_totales);
                if (B.modificarLibro(isbn, nb)) {
                    cout << "Libro modificado y CSV actualizado." << endl;
                }
                else{
                    cout << "No se pudo modificar." << endl;
                }
            } break;
            case 4: {
                // ---------------- OPCIÓN 4: Agregar usuario ----------------
                Usuario nu;
                nu.id_usuario = generar_id_aleatorio(12);
                cout << "ID del usuario generado: " << nu.id_usuario << endl;
                cout << "Nombre: ";
                getline(cin, nu.nombre);
                cout << "Correo: ";
                getline(cin, nu.correo);
                if (B.agregarUsuario(nu)) {
                    cout << "Usuario agregado con ID: " << nu.id_usuario << " y guardado en CSV." << endl;
                }
                else {
                    cout << "Error: ID generado ya existe (Intente de nuevo)." << endl;
                }
            } break;
            case 5: {
                // ---------------- OPCIÓN 5: Eliminar usuario ----------------
                string uid;
                cout << "ID de usuario: ";
                cin >> uid;
                if (B.eliminarUsuario(uid)) {
                    cout << "Usuario eliminado y CSV actualizado." << endl;
                }
                else {
                    cout << "No existe ese usuario." << endl;
                }
            } break;
            case 6: {
                // ---------------- OPCIÓN 6: Prestar libro ----------------
                string uid, isbn;
                cout << "ID de usuario: ";
                cin >> uid;
                cout << "ISBN: ";
                cin >> isbn;
                
                int resultado = B.prestamoLibro(uid, isbn);
                
                if (resultado == 1) {
                    cout << "Préstamo realizado exitosamente." << endl;
                }
                else if (resultado == 2) {
                    cout << "No hay copias disponibles. El usuario ha sido agregado a la LISTA DE ESPERA." << endl;
                }
                else if (resultado == 0) {
                    cout << "Error: El ISBN ingresado NO EXISTE en la biblioteca." << endl;
                }
                else if (resultado == -1) {
                    cout << "Error: El Usuario ingresado no existe." << endl;
                }
            } break;
            case 7: {
                // ---------------- OPCIÓN 7: Devolver libro ----------------
                string uid, isbn;
                cout << "ID de usuario: ";
                cin >> uid;
                cout << "ISBN: ";
                cin >> isbn;
                if (B.devolver_libro(uid, isbn)) {
                    cout << "Devolucion procesada. CSVs actualizados." << endl;
                }
                else {
                    cout << "Error al devolver. Verifique ID/ISBN." << endl;
                }
            } break;
            case 8: {
                // ---------------- OPCIÓN 8: Autocompletar título/autor ----------------
                string pref;
                cout << "Prefijo: ";
                getline(cin, pref);
                auto res = B.autocompletado_general(pref, 10);
                cout << "Sugerencias: " << endl;
                for (auto& t : res) {
                    cout << " - " << t << endl;
                }
            } break;
            case 9:
                // ---------------- OPCIÓN 9: Listar títulos ordenados ----------------
                B.mostrar_titulos_ordenados();
                break;
            case 10: {
            // ---------------- OPCIÓN 10: Recomendaciones por grafo ----------------
            string uid;
            cout << "Ingrese su ID de Usuario: ";
            cin >> uid;
            cin.ignore(); // Limpiar buffer antes del getline siguiente
            
            string genero_filtro = "";
            cout << "¿Desea filtrar recomendaciones por género? (Presione Enter para ver de todo): ";
            getline(cin, genero_filtro);
            
            // Llamamos al método del grafo
            // Si genero_filtro está vacío, recomienda de todo. Si tiene texto, filtra.
            auto recomendaciones = B.recomendar_para_usuario(uid, genero_filtro, 5); 
            
            cout << "--- Libros recomendados para " << uid << " ---" << endl;
            if (recomendaciones.empty()) {
                cout << "No hay suficientes datos en su historial o no hubo coincidencias." << endl;
            } else {
                for (auto& p : recomendaciones) {
                    cout << "* ISBN: " << p.first << " (Coincidencia: " << p.second << ")" << endl;
                    B.mostrar_libro(p.first);
                    cout << "--------------------------------" << endl;
                }
            }
        } break;
            case 11: {
                // ---------------- OPCIÓN 11: Ver libro ----------------
                string isbn;
                cout << "ISBN: ";
                cin >> isbn;
                B.mostrar_libro(isbn);
            } break;
            case 12: {
                // ---------------- OPCIÓN 12: Ver usuario ----------------
                string uid;
                cout << "ID de usuario: ";
                cin >> uid;
                B.mostrar_usuario(uid);
            } break;
            case 13:
                // ---------------- OPCIÓN 13: Listar por ISBN (AVL) ----------------
                B.mostrar_libros_por_isbn_num_ordenado();
            break;
            case 14:
            // ---------------- OPCIÓN 14: Deshacer última acción ----------------
            B.deshacerUltimaOperacion();
            break;
            case 15: {
            // ---------------- OPCIÓN 15: Buscar por género ----------------
            string genero;
            cout << "Ingrese el género que desea buscar: ";
            getline(cin, genero);
            B.mostrar_libros_por_genero(genero);
            } break;
            default:
                cout << "Opción inválida, seleccione algo más por favor" << endl;
                break;
        }
    }
    cout << "Saliendo..." << endl;
    return 0;
}
