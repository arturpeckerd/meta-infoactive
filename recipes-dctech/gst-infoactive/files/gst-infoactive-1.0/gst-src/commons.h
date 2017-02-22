#ifndef COMMONS_H_
#define COMMONS_H_

#define UI_SNDQUEUE_NAME             "/ui_sndqueue"
#define UI_RCVQUEUE_NAME             "/ui_rcvqueue"
#define MAX_SIZE                     64


#define MSG_UI_POSITION_UPDATE      "MSG_UI_POSITION_UPDATE"


#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

#define UNUSED(x) (void)(x)


#endif /* COMMONS_H_ */
