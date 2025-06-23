#pragma once
#include <string>
#include <glad/glad.h>

/**
 * @brief La classe Shader si occupa di caricare, compilare e linkare uno shader di tipo
 *        Vertex/Fragment in un unico Shader Program, fornendo metodi per l'uso
 *        e la gestione delle risorse.
 */
class Shader
{
public:
    /**
     * @brief Costruttore di default. Non carica immediatamente gli shader.
     */
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Distruttore: rilascia le risorse dello shader program.
     */
    ~Shader();

    /**
     * @brief Carica e compila i sorgenti degli shader vertex e fragment,
     *        quindi linka il program.
     *
     * @param vertexPath   Percorso del file sorgente Vertex Shader.
     * @param fragmentPath Percorso del file sorgente Fragment Shader.
     *
     * @throw std::runtime_error Se il file non può essere aperto o la compilazione/link fallisce.
     */
    void load(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Attiva (usa) questo shader program.
     */
    void use() const;

    void setInt(const std::string& name, GLint value) const;

    /**
     * @brief Restituisce l'ID del program OpenGL (glCreateProgram).
     *
     * @return GLuint Identificatore univoco del program.
     */
    GLuint getID() const { return m_programID; }

    /**
     * @brief Verifica se lo shader è stato correttamente caricato e linkato.
     *
     * @return true  Se lo shader è pronto per essere usato.
     * @return false Altrimenti.
     */
    bool isLoaded() const { return m_isLoaded; }

private:
    /**
     * @brief Legge il contenuto di un file di testo e lo restituisce come stringa.
     *
     * @param filePath Percorso del file di testo.
     * @return std::string Contenuto completo del file.
     *
     * @throw std::runtime_error Se il file non esiste o non può essere aperto.
     */
    std::string readFile(const std::string& filePath);

    /**
     * @brief Compila uno shader sorgente di tipo vertex o fragment.
     *
     * @param source     Sorgente dello shader (codice GLSL).
     * @param shaderType Tipo di shader (GL_VERTEX_SHADER o GL_FRAGMENT_SHADER).
     *
     * @return GLuint ID dello shader compilato (intermedio).
     *
     * @throw std::runtime_error Se la compilazione fallisce.
     */
    GLuint compileShader(const std::string& source, GLenum shaderType);

private:
    GLuint m_programID; ///< ID del programma shader creato con glCreateProgram()
    bool   m_isLoaded;  ///< Flag che indica se lo shader è correttamente caricato
};
