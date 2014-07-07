#include <Python.h>
#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

//Decoder methods
static PyObject* pypopro_decoder_init(PyObject *self, PyObject *args);
static PyObject* pypopro_decoder_read(PyObject *self, PyObject *args);
static PyObject* pypopro_decoder_close(PyObject *self, PyObject *args);

//Encoder methods
static PyObject* pypopro_encoder_init(PyObject *self, PyObject *args);
static PyObject* pypopro_encoder_read(PyObject *self, PyObject *args);
static PyObject* pypopro_encoder_close(PyObject *self, PyObject *args);

//Overlayer methods
static PyObject* pypopro_overlayer_init(PyObject *self, PyObject *args);
static PyObject* pypopro_overlayer_overlay(PyObject *self, PyObject *args);
static PyObject* pypopro_overlayer_close(PyObject *self, PyObject *args);


