#include "pypopro.h"
#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

static PyObject*
py_pypopro_decoder_init(PyObject *self, PyObject *args)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PypoproDecoder *decoder = pypopro_decoder_init(filename);

    return Py_BuildValue("l", (intptr_t) decoder);
}

static PyObject*
py_pypopro_decoder_read(PyObject *self, PyObject *args)
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
py_pypopro_decoder_close(PyObject *self, PyObject *args)
{
    long long decoder_long;
    if (!PyArg_ParseTuple(args, "L", &decoder_long))
        return NULL;

    PypoproDecoder *decoder = (PypoproDecoder *)(intptr_t) decoder_long;

    pypopro_decoder_close(decoder);

    Py_RETURN_NONE;
}

static PyObject*
py_pypopro_encoder_init(PyObject *self, PyObject *args)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PypoproEncoder *encoder = pypopro_encoder_init(filename);

    return Py_BuildValue("l", (intptr_t) encoder);
}

static PyObject*
py_pypopro_encoder_add_frame(PyObject *self, PyObject *args)
{
    long long encoder_long;
    long long frame_long;
    long long pts;
    if (!PyArg_ParseTuple(args, "LLL", &encoder_long, &frame_long, &pts))
        return NULL;

    PypoproEncoder *encoder = (PypoproEncoder *)(intptr_t) encoder_long;
    AVFrame *frame = (AVFrame *)(intptr_t) frame_long;

    pypopro_encoder_add_frame(encoder, frame, (int64_t) pts);

    Py_RETURN_NONE;
}




static PyObject*
py_pypopro_encoder_close(PyObject *self, PyObject *args)
{
    long long encoder_long;
    if (!PyArg_ParseTuple(args, "L", &encoder_long))
        return NULL;

    PypoproEncoder *encoder = (PypoproEncoder *)(intptr_t) encoder_long;

    pypopro_encoder_close(encoder);

    Py_RETURN_NONE;
}

static PyObject*
py_pypopro_overlayer_init(PyObject *self, PyObject *args)
{
    PypoproOverlayer *overlayer = pypopro_overlayer_init();

    return Py_BuildValue("l", (intptr_t) overlayer);
}

static PyObject*
py_pypopro_overlayer_overlay(PyObject *self, PyObject *args)
{
    long long overlayer_long;
    PyObject *py_frames;
    PyObject *py_widths;
    PyObject *py_heights;
    PyObject *py_posX;
    PyObject *py_posY;

    if (!PyArg_ParseTuple(args, "LO!O!O!O!O!",
                          &overlayer_long,
                          &PyList_Type, &py_frames,
                          &PyList_Type, &py_widths,
                          &PyList_Type, &py_heights,
                          &PyList_Type, &py_posX,
                          &PyList_Type, &py_posY))
    {
        return NULL;
    }


    PypoproOverlayer *overlayer = (PypoproOverlayer *)(intptr_t) overlayer_long;
    printf("overlayer-> %ld\n", (intptr_t) overlayer);

    PyObject *o;
    int num_frames = PyList_Size(py_frames);
    printf("overlay %d frames\n", num_frames);
    //TODO: check that lists have the same length
    
    AVFrame *frames[num_frames];
    int widths[num_frames];
    int heights[num_frames];
    int posX[num_frames];
    int posY[num_frames];
    for (int i = 0; i < num_frames; i++)
    {
        o = PyList_GetItem(py_frames, i);
        frames[i] = (AVFrame *)(intptr_t) PyLong_AsLong(o);

        o = PyList_GetItem(py_widths, i);
        widths[i] = (int) PyLong_AsLong(o);

        o = PyList_GetItem(py_heights, i);
        heights[i] = (int) PyLong_AsLong(o);

        o = PyList_GetItem(py_posX, i);
        posX[i] = (int) PyLong_AsLong(o);

        o = PyList_GetItem(py_posY, i);
        posY[i] = (int) PyLong_AsLong(o);

        printf("C: overlay %ld %d %d %d %d\n", frames[i], widths[i], heights[i], posX[i], posY[i]);
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
py_pypopro_overlayer_close(PyObject *self, PyObject *args)
{
    long long overlayer_long;
    if (!PyArg_ParseTuple(args, "L", &overlayer_long))
        return NULL;

    PypoproOverlayer *overlayer = (PypoproOverlayer *)(intptr_t) overlayer_long;

    pypopro_overlayer_close(overlayer);

    Py_RETURN_NONE;
}


static PyMethodDef
PypoproMethods[] = {
    {"decoder_init",  py_pypopro_decoder_init, METH_VARARGS, "Initialize a decoder."},
    {"decoder_read",  py_pypopro_decoder_read, METH_VARARGS, "Read a frame at a given PTS."},
    {"decoder_close",  py_pypopro_decoder_close, METH_VARARGS, "Close a decoder."},
    {"encoder_init",  py_pypopro_encoder_init, METH_VARARGS, "Initialize an encoder."},
    {"encoder_add_frame",  py_pypopro_encoder_add_frame, METH_VARARGS, "Add a frame to an encoder."},
    {"encoder_close",  py_pypopro_encoder_close, METH_VARARGS, "Close an encoder."},
    {"overlayer_init",  py_pypopro_overlayer_init, METH_VARARGS, "Initialize an overlayer."},
    {"overlayer_overlay",  py_pypopro_overlayer_overlay, METH_VARARGS, "Overlay frames with given dimensions and positions."},
    {"overlayer_close",  py_pypopro_overlayer_close, METH_VARARGS, "Close an overlayer."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


static struct PyModuleDef
pypopromodule = {
   PyModuleDef_HEAD_INIT,
   "pypopro",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   PypoproMethods
};

PyMODINIT_FUNC
PyInit_pypopro(void)
{
    return PyModule_Create(&pypopromodule);
}
