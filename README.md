PAV - P3: detección de pitch
============================

**Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.**

**Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.**

Ejercicios básicos
------------------

**- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa get_pitch.**

   * **Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.**


```c
void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
     //FILE *autocorr = fopen("autocorr.txt","a");
    for (unsigned int l = 0; l < r.size(); ++l) {
  		// \TODO Compute the autocorrelation r[l]
      r[l] = 0;
        for(unsigned int n = 0; n < x.size() - l; n++){
          r[l]= x[n] * x[n+l] + r[l];
        }
        r[l]=(1.0F/x.size())*r[l];
        //fprintf(autocorr,"%f\n",r[l]);
       
      /// \DONE 
    }
     //fclose(autocorr); 
    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
}

```

   * **Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de unos 30 ms de 	un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la autocorrelación de la señal y 	  la posición del primer máximo secundario.**

	 **NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para hacerlo. Se            valorará la utilización de la librería matplotlib de Python.**

Primero usamos `Matlab` para hacer la gráfica. Esta, se corresponde a la señal y a la autocorrelación de un audio de *30ms*, y en ellas hemos remarcado el **periodo de pitch** y el **primer máximo secundario.** 

<img src="imagenes/2.jpg" width="800" align="center">

Tambien hemos hecho la gráfica en **`Python`**, concretamente con la librería `matplotlib`, debido a las recomendaciones de nuestro profesor. 

<img src="imagenes/1.png" width="800" align="center">

*Nota:* Siendo una autocorrelación el máximo debería estar en x = 0, pero por algun motivo que no hemos podido solucionar, está en 600... 

A continuación se muestra un audio de más duración:

<img src="imagenes/3.jpg" width="800" align="center">

*Nota:* Este y los códigos siguientes de matlab y python se encuentran en la carpeta _pym_. 

   * **Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la autocorrelación. Inserte a continuación el código correspondiente.**

```c
while (*iR > 0 && iR < r.end()){
      iR++;
}

if(iR < r.begin() + npitch_min){ 
      iR = r.begin() + npitch_min;
}

iRMax = iR;
while(iR!=r.end()){
      if(*iR>*iRMax){
        iRMax=iR;
}
++iR;
```

   * **Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.**

Al principio, nuestro código para la decisión de sonoro o sordo se basaba únicamente en valores de correlaciones, como se ve a continuación:


```c
 bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You wan use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    bool unvoiced = true;
    if(r1norm > 0.95 || rmaxnorm > 0.48){
      unvoiced = false;
    }
    return unvoiced;
  }
```

Donde los umbrales los hemos ido optimizando a base de ir provando posibles valores. Como se puede observar, es una forma de decidir bastante senzilla y simple:  si estos valores estaban a un lado del umbral determinado, se decidía que el sonido era sordo, y si estaban al otro lado, se decidía que era sonoro. 

Más adelante, estuvimos pensando más posibles umbrales de decisión, y llegamos al siguiente código:

```c
bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm,float tasa) const {

    bool unvoiced = true;
    float potnorm=pot/potmaxima;
    //cout << potnorm << '\t' << pot << '\t' << potmaxima << endl;
    if(r1norm > umb1 || rmaxnorm > umb2 /*|| tasa < umb3*/){        
      unvoiced = false;
    }
    if(tasa > umb3){   // 4200 91,26%
      unvoiced = true;
    }
    if(potnorm<2.5 && tasa<2450){      //2.5  2500   91,53
      unvoiced = false;
    }
    return unvoiced;
}
```
Donde usamos:
   * Correlaciones
   * Tasa de cruces por cero
   * Potencia **normalizada**

Mucho mas completo. La tasa de cruces por cero sabemos, por teoría, que si es muy elevada el sonido muy posiblemente será sordo, ya que un sonido sordo, al tener menos amplitud y ser mas semejante al ruido, pasa mas veces por la recta y = 0 que un sonido sonoro. En el caso de correlación y tasa de cruces por cero, usamos `POSIX` para encontrar el valor óptimo. Esto, junto con la potencia normalizada, lo explicaremos en más detalle en el apartado de Ampliaciones. 


- **Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El resto del  	trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.**

  * **Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un segmento es sonoro o sordo.**
	
	  - **Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal (r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).**

	**Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.**
	**Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que en esta práctica es de 15 ms.**

Falta una gráfica que salga todo y explicar las relaciones entre todos. 

- **Use el detector de pitch implementado en el programa wavesurfer en una señal de prueba y compare su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica ilustrativa del resultado de ambos detectores.**

<img src="imagenes/5.png" width="800" align="center">

Otra forma de ver el resultado de nuestro código sería el siguiente (hemos usado otro audio):

<img src="imagenes/6.png" width="800" align="center">

En general podríamos concluir en que nuestro programa nos da un resultado bastante similar al de `Wavesurfer`, aunque en algunos puntos no acertamos del todo, están cerca del correcto. 

  * **Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos `pitch_db/train`..**

Unvoiced frames as voiced | Voiced frames as unvoiced | Gross voiced error | MSE of fine errors |
------------------------- | ------------------------- | ------------------ | ------------------ |
218/7045 (3.09 %)|356/4155 (8.57 %)|70/3799 (1.84 %)|2.73 %| 

Resultado TOTAL **91.54%**

<img src="imagenes/11.png" width="650" align="center">

   * **Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará el uso de alternativas de mayor calidad (particularmente Python).**
   
Como antes, hemos usado **Python**, con la libreria `Matplotlib`, y el resultado es el siguiente:

<img src="imagenes/7.jpeg" width="600" align="center">

La gráfica roja se corresponde al pitch que detecta nuestro programa, y la azul al pitch de referencia. Como se puede observar, son prácticamente iguales, hay tramos en los que se superponen completamente (es decir, la detección ha sido perfecta), y otros en los que difieren un poco, aunque tampoco demasiado. 

Ejercicios de ampliación
------------------------

### Docopt

- **Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del detector a   los argumentos de la línea de comandos.**
  
  **Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la base de datos.**

  * **Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización con los argumentos añadidos.**

##### Mensaje de ayuda del **docopt**:

```bash

get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt> 
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    --umbral1 float   [default: 0.95] r[1]/r[0]
    --umbral2 float   [default: 0.48] r[lag]/r[0]
    --umbral3 float   [default: 4200] tasa de cruces por cero

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0

```

#### Ejemplo de uso:

 * *Ejemplo 1*: Sin definir umbrales (los por defecto)
 
<img src="imagenes/9.png" width="500" align="center">


 * *Ejemplo 2*: Definiendo umbrales.
 
<img src="imagenes/10.png" width="750" align="center">

Gracias al docopt, tenemos un fácil y rápido acceso a modificar los valores de los umbrales con tal de ir optimizando el funcionamiento.


- **Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección de pitch.**
  **Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.**

### Center Clipping (Preprocesado)

```c
    float max=0.0F;
    for(unsigned int k = 0; k< x.size(); k++){
      if(x[k]> max) max = x[k];
    }
    float llindar=0.01*max;
    for(unsigned int k = 0; k < x.size(); k++){
      if (fabs(x[k]) < llindar) x[k]=0;
      else if (x[k]> llindar) x[k]-=llindar;
      else if (x[k] < -llindar) x[k]+=llindar;
    }
```
Para el preprocesado de la señal hemos incluido el *center clipping* en el código. Mantiene la periodicidad, reduciendo el ruido y evitando que los formantes puedan influir en la detección del pitch.

Con `Matlab`, hemos hecho un script (se encuentra en la carpeta `pym`) el cual hace un central clipping a 0.05:

<img src="clipping.jpeg/10.png" width="750" align="center">


### Filtro de Mediana (Postprocesado)

Para el postprocesado hemos usado un filtro de mediana, el código se muestra a continuación. 

```c
 filter = f0;

   min1=fmin(f0[0],f0[1]);
   max1= fmax(f0[0],f0[1]);
   filter[0]= f0[0]+f0[1]-min1-max1;

  for( unsigned int i=1; i < f0.size()-1 ; i++){

    min1=fmin(f0[i-1],f0[i]);
    min2=fmin(min1,f0[i+1]);
    max1= fmax(f0[i-1],f0[i]);
    max2 = fmax(max1,f0[i+1]);
    filter[i]= f0[i-1]+f0[i]+f0[i+1]-min2-max2; 
  }

  f0=filter;
```


### Optimización demostrable de los parámetros que gobiernan la decisión sonoro/sordo

#### Potencia Normalizada

En `get_pitch.cpp` en preprocesado, se devuelve la potencia máxima tal y como vemos en el siguiente código. 

```c
vector<float>::iterator iX;
  vector<float> f0;
  float pot_max = -1000.0F;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
        float aux=0.0F;
        for (int y = 0; y < n_len; y++){
            aux += iX[y]*iX[y];
            if(aux==0.0F){
              aux = 1e-10; 
            }
        }
        float PdB=10*log10(aux/n_len); 
        if(PdB>pot_max){
          pot_max=PdB;
        }
         //cout << PdB << '\t' << pot_max << endl;
 }
```

Es la misma forma que calculabamos la potencia en la práctica 1, solo que ahora buscamos el máximo de esta. 

Hacemos la llamada a `pitch_analizer.cpp` entonces, y añadimos las variables en `pitch_analizer.h.` 

En pitch_analizer.cpp calculamos la potencia:

```c
    float pot = 10 * log10(r[0]);
```

finalmente, en el método `unvoiced`, 

```c
    float potnorm=pot/potmaxima;
```
De esta forma, tenemos la **potencia normalizada** como umbral. 

Si la potencia normalizada es pequeña y la tasa es pequeña, entonces será sonoro.  

```c
if(potnorm<2.5 && tasa<2450){      
      unvoiced = false;
    }
return unvoiced;
```



### Bash 

Gracias al `docopt` hemos podido provar con más facilidad y rapidez los valores de los umbrales de correlación con tal de optimizar el funcionamiento de nuestro programa . Aún así, hemos intentado automatizar dicho proceso usando **`Bash`**. No nos funcionó y no hemos podido seguir intentándolo por falta de tiempo, pero dejamos aquí los códigos que hicimos. Nuestro objetivo era recorrer el valor de los umbrales de correlación de 0.00 a 1.00 en saltos de 0.05 en 0.05, por ejemplo. Guardar los valores en un fichero `.txt` y luego encontrar el máximo introduciendo en el terminal el siguiente comando `sort -t: -k2n results.txt | tail -1`. 

El primer intento de código actuaba únicamente en un audio, aunque sabemos que no es lo mejor... 

```bash
#! /usr/bin bash

for umbr1 in `seq 0.00 0.05 1.00`
do
    for umbr2 in `seq 0.00 0.05 1.00`
    do
        logrado=`get_pitch --umbr1 $umbr1 --umbr2 $umbr2 sb042.wav sb042.f0`
        echo -ne "$umbr1 | $umbr2 | $logrado" > results.txt
    done
done
```

El ideal sería el siguiente, que actuara sobre todos los audios y el valor exportado en el `.txt` fuera el de después de la palabra TOTAL (el %) con la línea `fgrep TOTAL > results.txt`

```bash
#! /usr/bin bash

for umbr1 in `seq 0.00 0.05 1.00`
do
    for umbr2 in `seq 0.00 0.05 1.00`
    do
        logrado=`get_pitch --umbr1 $umbr1 --umbr2 $umbr2 sb042.wav sb042.f0`
	`pitch_evaluate pitch_db/train/*f0ref`
    done
done | fgrep TOTAL > results.txt

```


Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
