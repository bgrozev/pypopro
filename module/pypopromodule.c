#include "pypopromodule.h"
#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

static PyObject*
pypopro_decoder_init(PyObject *self, PyObject *args)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PypoproDecoder *decoder = pypopro_decoder_init(filename);

    return Py_BuildValue("l", (intptr_t) decoder);
}

static PyObject*
pypopro_decoder_read(PyObject *self, PyObject *args)
{
    long long decoder_long;
    long long pts;
    if (!PyArg_ParseTuple(args, "LL", &decoder_long, &pts))
        return NULL;

    PypoproDecoder *decoder = (PypoproDecoder *)(intptr_t) decoder_long;

    AVFrame *frame = pypopro_decoder_read(decoder, (int64_t) pts);

    return Py_BuildValue("l", (intptr_t) frame);
}

static PyObject*
pypopro_decoder_close(PyObject *self, PyObject *args)
{
    long long decoder_long;
    if (!PyArg_ParseTuple(args, "L", &decoder_long))
        return NULL;

    PypoproDecoder *decoder = (PypoproDecoder *)(intptr_t) decoder_long;

    pypopro_decoder_close(decoder);

    Py_RETURN_NONE;
}

static PyObject*
pypopro_encoder_init(PyObject *self, PyObject *args)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PypoproEncoder *encoder = pypopro_encoder_init(filename);

    return Py_BuildValue("l", (intptr_t) encoder);
}

static PyObject*
pypopro_encoder_read(PyObject *self, PyObject *args)
{
    long long encoder_long;
    long long frame_long;
    long long pts;
    if (!PyArg_ParseTuple(args, "L:L", &encoder_long, &frame_long, &pts))
        return NULL;

    PypoproDecoder *decoder = (PypoproDecoder *)(intptr_t) decoder_long;
    AVFrame *frame = (AVFrame *)(intptr_t) frame;

    pypopro_encoder_add_frame(encoder, frame, (int64_t) pts);

    Py_RETURN_NONE;
}




static PyObject*
pypopro_encoder_close(PyObject *self, PyObject *args)
{
    long long encoder_long;
    if (!PyArg_ParseTuple(args, "L", &encoder_long))
        return NULL;

    PypoproEncoder *encoder = (PypoproEncoder *)(intptr_t) encoder_long;

    pypopro_encoder_close(encoder);

    Py_RETURN_NONE;
}

static PyObject*
pypopro_overlayer_init(PyObject *self, PyObject *args)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PypoproOverlayer *overlayer = pypopro_overlayer_init(filename);

    return Py_BuildValue("l", (intptr_t) overlayer);
}

static PyObject*
pypopro_overlayer_overlay(PyObject *self, PyObject *args)
{
    long long overlayer_long;
    PyObject *py_frames;
    PyObject *py_widths;
    PyObject *py_heights;
    PyObject *py_posX;
    PyObject *py_posY;

    if (!PyArg_ParseTuple(args, "LO!O!O!O!O!",
                          &overlayer_long,
                          &PyList_Type, py_frames,
                          &PyList_Type, py_widths,
                          &PyList_Type, py_heights,
                          &PyList_Type, py_posX,
                          &PyList_Type, py_posY))
        return NULL;

    PypoproOverlayer *overlayer = (PypoproOverlayer *)(intptr_t) overlayer_long;

    PyObject o;
    int num_frames = PyList_size(py_frames);
    //TODO: check that lists have the same length
    
    AVFrame **frames = malloc(num_frame * sizeof(AVFrame*));
    int widths[num_frames];
    int heights[num_frames];
    int posX[num_frames];
    int posY[num_frames];
    for (int i = 0; i < num_frames; i++)
    {
        o = PyList_GetItem(py_frames, i);
        frames[i] = (intptr_t) PyInt_AsLong(o);

        o = PyList_GetItem(py_widths, i);
        widths[i] = (intptr_t) PyInt_AsInt(o);

        o = PyList_GetItem(py_heights, i);
        heights[i] = (intptr_t) PyInt_AsInt(o);

        o = PyList_GetItem(py_posX, i);
        posX[i] = (intptr_t) PyInt_AsInt(o);

        o = PyList_GetItem(py_posY, i);
        posY[i] = (intptr_t) PyInt_AsInt(o);
    }


    AVFrame *frame = pypopro_overlayer_overlay(overlayer,
                                               frames,
                                               widths,
                                               heights,
                                               posX,
                                               posY,
                                               num_frames);

    return Py_BuildValue("l", (intptr_t) frame);
}

static PyObject*
pypopro_overlayer_close(PyObject *self, PyObject *args)
{
    long long overlayer_long;
    if (!PyArg_ParseTuple(args, "L", &overlayer_long))
        return NULL;

    PypoproOverlayer *overlayer = (PypoproOverlayer *)(intptr_t) overlayer_long;

    pypopro_overlayer_close(overlayer);

    Py_RETURN_NONE;
}


