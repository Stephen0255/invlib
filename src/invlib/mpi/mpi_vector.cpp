template
<
typename LocalType,
template <typename> typename StorageType
>
    template<typename>
MPIVector<LocalType, StorageType>::MPIVector()
    : local(), local_rows(0)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices.push_back(0);
        row_ranges.push_back(0);
    }

    m = 0;
}

template
<
typename LocalType,
template <typename> typename StorageType
>
    template<typename T, typename>
MPIVector<LocalType, StorageType>::MPIVector(T && local_vector)
    : local(local_vector), local_rows(local_vector.rows())
{
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices.push_back(index);
        row_ranges.push_back(proc_rows[i]);
        index += proc_rows[i];
    }

    m = index;
}

template
<
typename LocalType,
template <typename> typename StorageType
>
    template <typename>
auto MPIVector<LocalType, StorageType>::resize(unsigned int i)
    -> void
{
    m = i;

    // Distribute rows evenly over MPI processes.
    unsigned int total_rows = m;
    local_rows = total_rows / nprocs;
    unsigned int remainder = total_rows % nprocs;
    unsigned int local_start = local_rows * rank;

    if (rank < remainder)
    {
        local_rows += 1;
        local_start += rank;
    }
    else
    {
        local_start += remainder;
    }

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices[i] = index;
        row_ranges[i]  = proc_rows[i];
        index += proc_rows[i];
    }

    local.resize(local_rows);
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIVector<LocalType, StorageTemplate>::rows() const
    -> unsigned int
{
    return m;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
    template <typename>
auto MPIVector<LocalType, StorageTemplate>::get_local()
    -> LocalType &
{
    return local;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIVector<LocalType, StorageTemplate>::get_local() const
    -> const LocalType &
{
    return local;
}


template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIVector<LocalType, StorageTemplate>::operator()(unsigned int i) const
    -> RealType
{
    int owner;
    for (int r = 0; r < nprocs; r++)
    {
        if ((i >= row_indices[r]) && (i < row_indices[r] + row_ranges[r]))
            owner = r;
    }

    if (rank == owner)
        local_element = local(i - row_indices[rank]);

    MPI_Bcast(&local_element, 1, mpi_data_type, owner, MPI_COMM_WORLD);

    if (rank == owner)
        return local(i - row_indices[rank]);
    else
        return local_element;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIVector<LocalType, StorageTemplate>::operator()(unsigned int i)
    -> RealType &
{
    int owner = 0;
    for (int r = 0; r < nprocs; r++)
    {
        if ((i >= row_indices[r]) && (i < row_indices[r] + row_ranges[r]))
            owner = r;
    }

    if (rank == owner)
        local_element = local(i - row_indices[rank]);

    MPI_Bcast(&local_element, 1, mpi_data_type, owner, MPI_COMM_WORLD);

    if (rank == owner)
        return local(i - row_indices[rank]);
    else
        return local_element;
}


template
<
typename LocalType,
template <typename> typename StorageType
>
MPIVector<LocalType, StorageType>::operator LocalType() const
{
    LocalType v; v.resize(m);
    broadcast_local_block(v.raw_pointer(), local.raw_pointer());
    return v;
}

template
<
typename LocalType,
template <typename> typename StorageType
>
auto MPIVector<LocalType, StorageType>::broadcast_local_rows(int rows[]) const
    -> void
{
    rows[rank] = local_rows;
    for (unsigned int i = 0; i < nprocs; i++)
    {
        MPI_Bcast(rows + i, 1, MPI_INTEGER, i, MPI_COMM_WORLD);
    }
}

template
<
typename LocalType,
template <typename> typename StorageType
>
auto MPIVector<LocalType, StorageType>::broadcast_local_block(double *vector,
                                                 const double *block) const
    -> void
{
    memcpy(vector + row_indices[rank], block, row_ranges[rank] * sizeof(double));
    for (unsigned int i = 0; i < nprocs; i++)
    {
        MPI_Bcast(vector + row_indices[i], row_ranges[i], mpi_data_type,
                  i, MPI_COMM_WORLD);
    }
}
