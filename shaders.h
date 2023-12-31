#include "glm/glm.hpp"
#include "uniform.h"
#include "fragment.h"
#include "color.h"
#include "vertex.h"
#include "FastNoiseLite.h"
#include "framebuffer.h"
#include <cstdlib> // Necesario para std::rand()
#pragma once

void printMatrix(const glm::mat4& myMatrix) {
    // Imprimir los valores de la matriz
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << myMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Función para imprimir un vec4
void printVec4(const glm::vec4& vector) {
    std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
}

// Función para imprimir un vec3
void printVec3(const glm::vec3& vector) {
    std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
}


Vertex vertexShader(const Vertex& vertex, const Uniform& uniform) {
    glm::vec4 transformedVertex = uniform.viewport * uniform.projection * uniform.view * uniform.model * glm::vec4(vertex.position, 1.0f);
    glm::vec3 vertexRedux;
    vertexRedux.x = transformedVertex.x / transformedVertex.w;
    vertexRedux.y = transformedVertex.y / transformedVertex.w;
    vertexRedux.z = transformedVertex.z / transformedVertex.w;
    Color fragmentColor(255, 0, 0, 255);
    glm::vec3 normal = glm::normalize(glm::mat3(uniform.model) * vertex.normal);
    Fragment fragment;
    fragment.position = glm::ivec2(transformedVertex.x, transformedVertex.y);
    fragment.color = fragmentColor;
    return Vertex {vertexRedux, normal, vertex.tex, vertex.position};
}


Color fragmentShaderTierra(Fragment& fragment) {
    Color groundColor(34, 139, 34);
    Color oceanColor(0, 0, 139);
    Color cloudColor(255, 255, 255);

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 50.0f; // Ajusta el valor según tu preferencia

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    FastNoiseLite noiseGenerator2;
    noiseGenerator2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox2 = 3000.0f;
    float oy2 = 5000.0f;
    float zoom2 = 450.0f; // Ajusta el valor para nubes más pequeñas

    float noiseValue2 = noiseGenerator2.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);

    // Combinar los dos niveles de ruido
    noiseValue = (noiseValue + noiseValue2) * 1.0f; // Ajusta el factor de mezcla según tus preferencias

    Color tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

    float oxc = 5500.0f;
    float oyc = 6900.0f;
    float zoomc = 1000.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    if (noiseValueC > 0.7f) { // Ajusta el umbral para nubes más pequeñas
        tmpColor = cloudColor;
    }

    fragment.color = tmpColor * fragment.z * fragment.intensity;

    return fragment.color;

}

Color fragmentShaderMarte(Fragment& fragment) {
    // Base color of the sun (amarillo)
    Color baseColorYellow(255, 204, 0); // Color en formato Color (RGB)

    // Otro color (rojo)
    Color baseColorRed(255, 0, 0);

    // Coeficiente de mezcla entre los dos colores (0.0 para amarillo puro, 1.0 para rojo puro)
    float mixFactor = 0.5; // Puedes ajustar este valor según tus preferencias

    // Aplica un gradiente desde el centro hacia el borde para dar una sensación de profundidad
    float distanceFromCenter = glm::length(fragment.original);
    float gradientFactor = 5.0f - distanceFromCenter;

    // Aplica un factor de intensidad para ajustar el brillo
    float intensity = 0.5f;

    // Combina los dos colores manualmente
    Color mixedColor;
    mixedColor.r = (1.0 - mixFactor) * baseColorYellow.r + mixFactor * baseColorRed.r;
    mixedColor.g = (1.0 - mixFactor) * baseColorYellow.g + mixFactor * baseColorRed.g;
    mixedColor.b = (1.0 - mixFactor) * baseColorYellow.b + mixFactor * baseColorRed.b;

    // Combina el color mezclado con el gradiente y la intensidad
    Color sunColor = mixedColor * gradientFactor * intensity;

    fragment.color = sunColor;

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 500.0f;
    float oy = 500.0f;
    float zoom = 5000.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    FastNoiseLite noiseGenerator2;
    noiseGenerator2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox2 = 3000.0f;
    float oy2 = 5000.0f;
    float zoom2 = 5000.0f; // Ajusta el valor para nubes más pequeñas

    float noiseValue2 = noiseGenerator2.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);

    // Combinar los dos niveles de ruido
    noiseValue = (noiseValue + noiseValue2) * 10.0f; // Ajusta el factor de mezcla según tus preferencias

    Color tmpColor = (noiseValue < 0.5f) ? baseColorRed: sunColor;

    if (noiseValue > 0.5){
        tmpColor = sunColor;
    }

    fragment.color = tmpColor * fragment.z * fragment.intensity;

    return fragment.color;
}
// Fragment shader para el sol
Color fragmentShaderSol(Fragment& fragment) {
    // Base color del sol (amarillo)
    Color baseColorYellow(255, 100, 0);

    // Color de la textura de fuego (rojo)
    Color fireColor(255, 0, 0); // Ajusta este color a tu preferencia de rojo

    // Coeficiente de mezcla entre el color base y la textura de fuego
    float mixFactor = 0.1; // Ajusta según tus preferencias

    // Distancia desde el centro para el gradiente
    float distanceFromCenter = length(fragment.original);
    float gradientFactor = 5.0 - distanceFromCenter;

    // Intensidad para ajustar el brillo
    float intensity = 15.0;

    // Mezcla entre el color base y la textura de fuego
    Color mixedColor;
    mixedColor.r = (1.0 - mixFactor) * baseColorYellow.r + mixFactor * fireColor.r;
    mixedColor.g = (1.0 - mixFactor) * fireColor.g + mixFactor * fireColor.g;
    mixedColor.b = (1.0 - mixFactor) * baseColorYellow.b + mixFactor * baseColorYellow.b;

    // Aplica el gradiente, la intensidad y el color base mezclado
    Color sunColor = mixedColor * gradientFactor * intensity;

    // Obtén las coordenadas UV del fragmento
    float uvX = fragment.original.x;
    float uvY = fragment.original.y;

    // Parámetros para el ruido
    float noiseScale = 0.1; // Ajusta según tus preferencias
    float noiseIntensity = 0.3; // Ajusta según tus preferencias

    // Genera ruido para la textura de fuego
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float noiseValue = noiseGenerator.GetNoise((uvX + 500.0) * 5000.0, (uvY + 500.0) * 5000.0) * noiseScale;
    float noiseValue2 = noiseGenerator.GetNoise((uvX + 3000.0) * 5000.0, (uvY + 5000.0) * 5000.0) * (noiseScale * 0.5);

    // Combinación de los dos niveles de ruido
    noiseValue = (noiseValue + noiseValue2) * 10.0; // Ajusta según tus preferencias

    // Mezcla entre el color base y la textura de fuego según el valor de ruido
    Color finalColor = sunColor * (1.0 - noiseValue) + fireColor * noiseValue;

    // Aplica la intensidad y la profundidad del fragmento
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}

Color fragmentShaderMorado(Fragment& fragment) {
    // Color de la textura de fuego (rojo)
    Color fireColor(500, 0, 200); // Ajusta este color a tu preferencia de rojo

    // Coeficiente de mezcla entre el color base y la textura de fuego

    // Distancia desde el centro para el gradiente
    float distanceFromCenter = length(fragment.original);
    float gradientFactor = 1.0 - distanceFromCenter;

    // Intensidad para ajustar el brillo
    float intensity = 1.0;
    // Aplica el gradiente, la intensidad y el color base mezclado
    Color sunColor = fireColor * gradientFactor * intensity;

    // Obtén las coordenadas UV del fragmento
    float uvX = fragment.original.x;
    float uvY = fragment.original.y;

    // Parámetros para el ruido
    float noiseScale = 0.1; // Ajusta según tus preferencias

    // Genera ruido para la textura de fuego
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float noiseValue = noiseGenerator.GetNoise((uvX + 5000.0) * 900.0, (uvY + 5000.0) * 900.0) * noiseScale;
    float noiseValue2 = noiseGenerator.GetNoise((uvX + 3000.0) * 900.0, (uvY + 5000.0) * 900.0) * (noiseScale * 0.5);

    // Combinación de los dos niveles de ruido
    noiseValue = (noiseValue + noiseValue2) * 50.0; // Ajusta según tus preferencias

    // Mezcla entre el color base y la textura de fuego según el valor de ruido
    Color finalColor = sunColor * (1.0 - noiseValue) + fireColor * noiseValue;

    // Aplica la intensidad y la profundidad del fragmento
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}

Color fragmentShaderNeptuno(Fragment& fragment) {
    // Intensidad de las líneas
    float lineIntensity = 0.7; // Ajusta según tus preferencias

    // Número de líneas
    int numLines = 3; // Ajusta según tus preferencias

    // Color de Neptuno (azul cian oscuro)
    Color neptuneColor(0, 51, 153);
    Color black(0, 0, 102);

    // Calcula la posición vertical normalizada en el rango [0, 1]
    float normalizedY = (fragment.original.y + 1.0) * 1.0;

    // Calcula el patrón de líneas horizontales
    float linePattern = glm::fract(normalizedY * numLines);

    // Aplica el efecto de líneas difuminadas
    float blur = 1.0 - abs(linePattern - 0.5) * 1.2
    ;
    blur = pow(blur, lineIntensity);

    // Combina el color de Neptuno con el efecto de líneas difuminadas
    Color finalColor = neptuneColor * blur + black * (1.0 - blur);

    fragment.color = finalColor;

    return fragment.color;
}

Color fragmentShaderJupiter(Fragment& fragment) {
    // Intensidad de las líneas
    float lineIntensity = 1.0; // Ajusta según tus preferencias

    // Número de líneas
    int numLines = 5; // Ajusta según tus preferencias

    // Color de Neptuno (azul cian oscuro)
    Color neptuneColor(249, 151, 80);
    Color black(195, 96, 55);

    // Calcula la posición vertical normalizada en el rango [0, 1]
    float normalizedY = (fragment.original.y + 1.0) * 1.0;

    // Calcula el patrón de líneas horizontales
    float linePattern = glm::fract(normalizedY * numLines);

    // Aplica el efecto de líneas difuminadas
    float blur = 1.0 - abs(linePattern - 0.5) * 1.5;
    blur = pow(blur, lineIntensity);

    // Combina el color de Neptuno con el efecto de líneas difuminadas
    Color finalColor = neptuneColor * blur + black * (1.0 - blur);

    fragment.color = finalColor;

    return fragment.color;
}


Color fragmentShaderLuna(Fragment& fragment) {
    // Tamaño de los cráteres
    float craterSize = 0.05; // Ajusta según tus preferencias

    // Intensidad de los cráteres
    float craterIntensity = 0.3; // Ajusta según tus preferencias

    // Color de fondo (gris claro)
    Color backgroundColor(200, 200, 200);

    // Color de los cráteres (gris oscuro)
    Color craterColor(50, 50, 50);

    // Calcula las coordenadas UV normalizadas
    float uvX = (fragment.original.x + 1.0) * 0.5;
    float uvY = (fragment.original.y + 1.0) * 0.5;

    // Calcula un patrón de cráteres sin clamp ni mix
    float centerX = 0.5;
    float centerY = 0.5;
    float distance = sqrt((uvX - centerX) * (uvX - centerX) + (uvY - centerY) * (uvY - centerY));
    float craterPattern = (distance < craterSize) ? 1.0 : 0.0;

    // Aplica el efecto de cráteres manualmente
    float craterEffect = craterPattern * craterIntensity;

    // Combina el color de fondo con el color de los cráteres manualmente
    Color finalColor = backgroundColor * (1.0 - craterEffect) + craterColor * craterEffect;

    fragment.color = finalColor;

    return fragment.color;
}
