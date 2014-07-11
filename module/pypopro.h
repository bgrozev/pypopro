#include <Python.h>
#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

//Decoder methods
static PyObject* py_pypopro_decoder_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_decoder_read(PyObject *self, PyObject *args);
static PyObject* py_pypopro_decoder_close(PyObject *self, PyObject *args);

//Encoder methods
static PyObject* py_pypopro_encoder_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_encoder_add_frame(PyObject *self, PyObject *args);
static PyObject* py_pypopro_encoder_close(PyObject *self, PyObject *args);

//Scaler methods
static PyObject* py_pypopro_scaler_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_scaler_scale(PyObject *self, PyObject *args);
static PyObject* py_pypopro_scaler_close(PyObject *self, PyObject *args);

//Overlayer methods
static PyObject* py_pypopro_overlayer_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_overlayer_overlay(PyObject *self, PyObject *args);
static PyObject* py_pypopro_overlayer_close(PyObject *self, PyObject *args);

//FFMPEG wrappers
static PyObject* py_pypopro_get_dimensions(PyObject *self, PyObject *args);
