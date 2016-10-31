#ifndef IO_READERS_H
#define IO_READERS_H

#include <fstream>
#include <iostream>
#include <string>

#include "pugixml.hpp"
#include "pugixml.cpp"
#include "endian.h"

namespace invlib
{

template <typename T> T read_matrix_arts(const std::string &);

template <>
EigenSparse read_matrix_arts<EigenSparse>(const std::string & filename)
{
    // Read xml file.
    pugi::xml_document doc;
    doc.load_file(filename.c_str());
    pugi::xml_node node = doc.document_element();

    // Binary or text format?
    std::string format = node.attribute("format").value();

    // Rows and columns.
    pugi::xml_node sparse_node    = node.child("Sparse");
    std::string rows_string = sparse_node.attribute("nrows").value();
    std::string cols_string = sparse_node.attribute("ncols").value();

    pugi::xml_node rind_node       = sparse_node.child("RowIndex");
    pugi::xml_node cind_node       = sparse_node.child("ColIndex");
    pugi::xml_node data_node       = sparse_node.child("SparseData");

    std::string nelem_string = rind_node.attribute("nelem").value();

    size_t nelem = std::stoi(nelem_string);
    std::vector<Eigen::Triplet<double>> triplets{};
    triplets.reserve(nelem);

    if (format == "ascii")
    {
        std::stringstream rind_stream(rind_node.child_value());
        std::stringstream cind_stream(cind_node.child_value());
        std::stringstream data_stream(data_node.child_value());

        std::string cind_string, rind_string, data_string;
        size_t rind, cind;
        double data;

        for (size_t i = 0; i < nelem; i++)
        {
            cind_stream >> cind;
            rind_stream >> rind;
            data_stream >> data;
            triplets.emplace_back(cind, rind, data);
        }
    }
    else if (format == "binary")
    {
        union {
            unsigned char buf[8];
            uint32_t      four;
            uint64_t      eight;
        } buf;

        unsigned int rind, cind;
        double data;

        std::ifstream stream(filename + ".bin", std::ios::in | std::ios::binary);
        Eigen::Triplet<double> triplet{};

        if (stream.is_open())
        {
            // Read indices byte by byte and convert from little endian format.
            for (size_t i = 0; i < nelem; i++)
            {
                buf.buf[0] = stream.get();
                buf.buf[1] = stream.get();
                buf.buf[2] = stream.get();
                buf.buf[3] = stream.get();

                rind = le32toh(buf.four);
                triplets.emplace_back(0, rind, 0.0);
            }
            // Read indices byte by byte and convert from little endian format.
            for (size_t i = 0; i < nelem; i++)
            for (size_t i = 0; i < nelem; i++)
            {
                buf.buf[0] = stream.get();
                buf.buf[1] = stream.get();
                buf.buf[2] = stream.get();
                buf.buf[3] = stream.get();
                cind = le32toh(buf.four);
                triplets[i] = Eigen::Triplet<double>(cind, triplets[i].row(), 0.0);
            }
            // Read data byte by byte and convert from little endian format.
            for (size_t i = 0; i < nelem; i++)
            {
                buf.buf[0] = stream.get();
                buf.buf[1] = stream.get();
                buf.buf[2] = stream.get();
                buf.buf[3] = stream.get();
                buf.buf[4] = stream.get();
                buf.buf[5] = stream.get();
                buf.buf[6] = stream.get();
                buf.buf[7] = stream.get();

                uint64_t host_endian = be64toh(buf.eight);
                data = *reinterpret_cast<double*>(&host_endian);
                triplets[i] = Eigen::Triplet<double>(cind, triplets[i].row(), 0.0);
                triplets[i] = Eigen::Triplet<double>(triplets[i].col(),
                                                     triplets[i].row(), data);
            }
        }
    }

    EigenSparse matrix; matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}


template <typename T> T read_vector_arts(const std::string &);

template <>
EigenVector read_vector_arts<EigenVector>(const std::string & filename)
{
    // Read xml file.
    pugi::xml_document doc;
    doc.load_file(filename.c_str());
    pugi::xml_node node = doc.document_element();

    // Binary or text format?
    std::string format = node.attribute("format").value();

    // Number of elements.
    pugi::xml_node sparse_node    = node.child("Vector");

    pugi::xml_node vector_node = sparse_node.child("Vector");
    std::string nelem_string   = vector_node.attribute("nelem").value();

    size_t nelem = std::stoi(nelem_string);

    EigenVector v; v.resize(nelem);

    if (format == "ascii")
    {
        std::stringstream elem_stream(vector_node.child_value());
        double data;

        for (size_t i = 0; i < nelem; i++)
        {
            elem_stream >> data;
            v[i] = data;
        }
    }
    else if (format == "binary")
    {
        union {
            unsigned char buf[8];
            uint64_t      eight;
        } buf;

        double data;

        std::ifstream stream(filename + ".bin", std::ios::in | std::ios::binary);

        if (stream.is_open())
        {
            // Read data byte by byte and convert from little endian format.
            for (size_t i = 0; i < nelem; i++)
            {
                buf.buf[0] = stream.get();
                buf.buf[1] = stream.get();
                buf.buf[2] = stream.get();
                buf.buf[3] = stream.get();
                buf.buf[4] = stream.get();
                buf.buf[5] = stream.get();
                buf.buf[6] = stream.get();
                buf.buf[7] = stream.get();

                uint64_t host_endian = be64toh(buf.eight);
                data = *reinterpret_cast<double*>(&host_endian);
                v[i] = data;
            }
        }
    }

    return v;
}

} // namespace invlib

#endif

