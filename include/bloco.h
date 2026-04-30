#ifndef BLOCO_H
#define BLOCO_H

typedef enum {
    BLOCO_TIPO_NORMAL  = 0,
    BLOCO_TIPO_POWERUP
} TipoBloco;

typedef enum {
    BLOCO_ESTADO_ATIVO = 0,
    BLOCO_ESTADO_USADO
} EstadoBloco;

typedef struct {
    TipoBloco   tipo;
    EstadoBloco estado;
    float       cafeOffset;   // 0 = no topo do bloco, -TILE = acima do bloco
    int         cafeColetado;
} Bloco;

#endif
