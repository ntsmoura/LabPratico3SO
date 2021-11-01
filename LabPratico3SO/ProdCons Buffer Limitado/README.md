# Produtor/Consumidor no Controle de Buffers de Capacidade Limitada

## Instruções de uso

O script "buffer.sh" foi criado para facilitar a compilação e execução do código.

A primeira linha do script refere-se à compilação com as devidas diretivas.

A segunda linha do script refere-se à chamada de execução do resultado da compilação.

A chamada de execução deve ser feita digitando-se no terminal **./buffer X Y Z B1 B2**, com todos os argumentos sendo valores inteiros positivos que representam o seguinte:

* **X** representa a quantidade de threads *X* (produtoras para o buffer *B1*);
* **Y** representa a quantidade de threads *Y* (consumidoras do buffer *B1* e produtoras para o buffer *B2*);
* **Z** representa a quantidade de threads *Z* (consumidoras do buffer *B2*);
* **B1** representa a quantidade de vagas no buffer *B1*;
* **B2** representa a quantidade de vagas no buffer *B2*.
