#include <glad/glad.h>
#include <core/utils/vector.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine {
    
    struct Vertex {
        Engine::Vector3 position;
        Engine::Vector3 normal;
        Engine::Vector2 texCoord;
        
        Vertex() : position(0), normal(0), texCoord(0) {}
        Vertex(const Engine::Vector3& pos) : position(pos), normal(0), texCoord(0) {}
        Vertex(const Engine::Vector3& pos, const Engine::Vector3& norm) : position(pos), normal(norm), texCoord(0) {}
        Vertex(const Engine::Vector3& pos, const Engine::Vector3& norm, const Engine::Vector2& uv) 
            : position(pos), normal(norm), texCoord(uv) {}
    };
    
    class Mesh {
    public:
        Mesh() {
            glGenVertexArrays(1, &m_VAO);
            glGenBuffers(1, &m_VBO);
        }
        
        explicit Mesh(const std::vector<Vertex>& vertices) : Mesh() {
            SetVertices(vertices);
        }
        
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) : Mesh() {
            SetVerticesAndIndices(vertices, indices);
        }
        
        ~Mesh() {
            Cleanup();
        }
        
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        
        Mesh(Mesh&& other) noexcept 
            : m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO),
              m_VertexCount(other.m_VertexCount), m_IndexCount(other.m_IndexCount),
              m_HasNormals(other.m_HasNormals), m_HasTexCoords(other.m_HasTexCoords) {
            other.m_VAO = 0;
            other.m_VBO = 0;
            other.m_EBO = 0;
            other.m_VertexCount = 0;
            other.m_IndexCount = 0;
        }
        
        Mesh& operator=(Mesh&& other) noexcept {
            if (this != &other) {
                Cleanup();
                
                m_VAO = other.m_VAO;
                m_VBO = other.m_VBO;
                m_EBO = other.m_EBO;
                m_VertexCount = other.m_VertexCount;
                m_IndexCount = other.m_IndexCount;
                m_HasNormals = other.m_HasNormals;
                m_HasTexCoords = other.m_HasTexCoords;
                
                other.m_VAO = 0;
                other.m_VBO = 0;
                other.m_EBO = 0;
                other.m_VertexCount = 0;
                other.m_IndexCount = 0;
            }
            return *this;
        }
        
        void SetVertices(const std::vector<Vertex>& vertices) {
            m_VertexCount = vertices.size();
            m_IndexCount = 0;
            
            m_HasNormals = true;
            m_HasTexCoords = true;
            for (const auto& vertex : vertices) {
                if (vertex.normal.x == 0 && vertex.normal.y == 0 && vertex.normal.z == 0) {
                    m_HasNormals = false;
                }
                if (vertex.texCoord.x == 0 && vertex.texCoord.y == 0) {
                    m_HasTexCoords = false;
                }
            }
            
            glBindVertexArray(m_VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            
            if (m_EBO != 0) {
                glDeleteBuffers(1, &m_EBO);
                m_EBO = 0;
            }
            
            SetupVertexAttributes();
            
            glBindVertexArray(0);
        }
        
        void SetIndices(const std::vector<unsigned int>& indices) {
            m_IndexCount = indices.size();
            
            if (m_EBO == 0) {
                glGenBuffers(1, &m_EBO);
            }
            
            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glBindVertexArray(0);
        }
        
        void SetVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
            SetVertices(vertices);
            SetIndices(indices);
        }
        
        void SetPositions(const std::vector<Engine::Vector3>& positions) {
            std::vector<Vertex> vertices(positions.size());
            for (size_t i = 0; i < positions.size(); ++i) {
                vertices[i].position = positions[i];
            }
            SetVertices(vertices);
        }
        
        void SetNormals(const std::vector<Engine::Vector3>& normals) {
            if (m_VertexCount != normals.size()) {
                std::cerr << "Mesh::SetNormals: Vertex count mismatch!" << std::endl;
                return;
            }
            
            std::vector<Vertex> vertices(m_VertexCount);
            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCount * sizeof(Vertex), vertices.data());
            
            for (size_t i = 0; i < m_VertexCount; ++i) {
                vertices[i].normal = normals[i];
            }
            
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCount * sizeof(Vertex), vertices.data());
            glBindVertexArray(0);
            
            m_HasNormals = true;
        }
        
        void SetTexCoords(const std::vector<Engine::Vector2>& texCoords) {
            if (m_VertexCount != texCoords.size()) {
                std::cerr << "Mesh::SetTexCoords: Vertex count mismatch!" << std::endl;
                return;
            }
            
            std::vector<Vertex> vertices(m_VertexCount);
            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCount * sizeof(Vertex), vertices.data());
            
            for (size_t i = 0; i < m_VertexCount; ++i) {
                vertices[i].texCoord = texCoords[i];
            }
            
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCount * sizeof(Vertex), vertices.data());
            glBindVertexArray(0);
            
            m_HasTexCoords = true;
        }
        
        void Draw() const {
            if (!IsValid()) return;
            
            glBindVertexArray(m_VAO);
            
            if (m_IndexCount > 0) {
                glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
            }
            
            glBindVertexArray(0);
        }
        
        void DrawInstanced(unsigned int instanceCount) const {
            if (!IsValid()) return;
            
            glBindVertexArray(m_VAO);
            
            if (m_IndexCount > 0) {
                glDrawElementsInstanced(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0, instanceCount);
            } else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, m_VertexCount, instanceCount);
            }
            
            glBindVertexArray(0);
        }
        
        unsigned int GetVertexCount() const { return m_VertexCount; }
        unsigned int GetIndexCount() const { return m_IndexCount; }
        bool HasIndices() const { return m_IndexCount > 0; }
        bool IsValid() const { return m_VAO != 0 && m_VertexCount > 0; }
        
        void Cleanup() {
            if (m_VAO != 0) {
                glDeleteVertexArrays(1, &m_VAO);
                m_VAO = 0;
            }
            if (m_VBO != 0) {
                glDeleteBuffers(1, &m_VBO);
                m_VBO = 0;
            }
            if (m_EBO != 0) {
                glDeleteBuffers(1, &m_EBO);
                m_EBO = 0;
            }
            m_VertexCount = 0;
            m_IndexCount = 0;
        }
        
    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_EBO = 0;
        
        unsigned int m_VertexCount = 0;
        unsigned int m_IndexCount = 0;
        
        bool m_HasNormals = false;
        bool m_HasTexCoords = false;
        
        void SetupVertexAttributes() {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            
            if (m_HasNormals) {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            }
            
            if (m_HasTexCoords) {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
            }
        }
    };
    
    class Shader {
    public:
        Shader() : m_ID(0) {}
        
        ~Shader() {
            Cleanup();
        }
        
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        
        Shader(Shader&& other) noexcept : m_ID(other.m_ID) {
            other.m_ID = 0;
        }
        
        Shader& operator=(Shader&& other) noexcept {
            if (this != &other) {
                Cleanup();
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }
        
        bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
            return LoadFromFiles(vertexPath, fragmentPath, "");
        }
        
        bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
            std::string vertexCode;
            std::string fragmentCode;
            std::string geometryCode;
            
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::ifstream gShaderFile;
            
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            
            try {
                vShaderFile.open(vertexPath);
                std::stringstream vShaderStream;
                vShaderStream << vShaderFile.rdbuf();
                vShaderFile.close();
                vertexCode = vShaderStream.str();
                
                fShaderFile.open(fragmentPath);
                std::stringstream fShaderStream;
                fShaderStream << fShaderFile.rdbuf();
                fShaderFile.close();
                fragmentCode = fShaderStream.str();
                
                if (!geometryPath.empty()) {
                    gShaderFile.open(geometryPath);
                    std::stringstream gShaderStream;
                    gShaderStream << gShaderFile.rdbuf();
                    gShaderFile.close();
                    geometryCode = gShaderStream.str();
                }
            } catch (std::ifstream::failure& e) {
                std::cerr << "Shader::LoadFromFiles: Failed to read shader files!" << std::endl;
                return false;
            }
            
            return LoadFromSource(vertexCode, fragmentCode, geometryCode);
        }
        
        bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
            return LoadFromSource(vertexSource, fragmentSource, "");
        }
        
        bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource) {
            unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
            if (vertexShader == 0) return false;
            
            unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
            if (fragmentShader == 0) {
                glDeleteShader(vertexShader);
                return false;
            }
            
            unsigned int geometryShader = 0;
            if (!geometrySource.empty()) {
                geometryShader = CompileShader(GL_GEOMETRY_SHADER, geometrySource);
                if (geometryShader == 0) {
                    glDeleteShader(vertexShader);
                    glDeleteShader(fragmentShader);
                    return false;
                }
            }
            
            bool linked = LinkProgram(vertexShader, fragmentShader, geometryShader);
            
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            if (geometryShader != 0) {
                glDeleteShader(geometryShader);
            }
            
            return linked;
        }
        
        void Use() const {
            if (m_ID != 0) {
                glUseProgram(m_ID);
            }
        }
        
        void Unuse() const {
            glUseProgram(0);
        }
        
        void SetBool(const std::string& name, bool value) const {
            SetInt(name, (int)value);
        }
        
        void SetInt(const std::string& name, int value) const {
            glUniform1i(GetUniformLocation(name), value);
        }
        
        void SetFloat(const std::string& name, float value) const {
            glUniform1f(GetUniformLocation(name), value);
        }
        
        void SetFloat2(const std::string& name, const Engine::Vector2& value) const {
            glUniform2f(GetUniformLocation(name), value.x, value.y);
        }
        
        void SetFloat3(const std::string& name, const Engine::Vector3& value) const {
            glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
        }
        
        void SetFloat4(const std::string& name, const Engine::Vector4& value) const {
            glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
        }
        
        void SetMatrix3(const std::string& name, const float* value, bool transpose = false) const {
            glUniformMatrix3fv(GetUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, value);
        }
        
        void SetMatrix4(const std::string& name, const float* value, bool transpose = false) const {
            glUniformMatrix4fv(GetUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, value);
        }
        
        unsigned int GetID() const { return m_ID; }
        bool IsValid() const { return m_ID != 0; }
        
        void Cleanup() {
            if (m_ID != 0) {
                glDeleteProgram(m_ID);
                m_ID = 0;
            }
        }
        
    private:
        unsigned int m_ID = 0;
        
        unsigned int CompileShader(unsigned int type, const std::string& source) {
            unsigned int shader = glCreateShader(type);
            const char* src = source.c_str();
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);
            
            int success;
            char infoLog[1024];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Shader compilation error (" 
                          << (type == GL_VERTEX_SHADER ? "VERTEX" : 
                              (type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY")) 
                          << "):\n" << infoLog << std::endl;
                glDeleteShader(shader);
                return 0;
            }
            
            return shader;
        }
        
        bool LinkProgram(unsigned int vertexShader, unsigned int fragmentShader, unsigned int geometryShader) {
            if (m_ID != 0) {
                glDeleteProgram(m_ID);
            }
            
            m_ID = glCreateProgram();
            glAttachShader(m_ID, vertexShader);
            glAttachShader(m_ID, fragmentShader);
            
            if (geometryShader != 0) {
                glAttachShader(m_ID, geometryShader);
            }
            
            glLinkProgram(m_ID);
            
            int success;
            char infoLog[1024];
            glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(m_ID, 1024, nullptr, infoLog);
                std::cerr << "Shader linking error:\n" << infoLog << std::endl;
                glDeleteProgram(m_ID);
                m_ID = 0;
                return false;
            }
            
            return true;
        }
        
        int GetUniformLocation(const std::string& name) const {
            int location = glGetUniformLocation(m_ID, name.c_str());
            if (location == -1) {
                std::cerr << "Uniform '" << name << "' not found in shader!" << std::endl;
            }
            return location;
        }
    };
    
}