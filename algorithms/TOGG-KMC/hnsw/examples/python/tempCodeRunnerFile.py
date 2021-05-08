  t = HnswIndex(f, "L2")  # HnswIndex(f, "L2 or angular")

    for data in datas:
        t.add_data(data)

    

    t.build(m=16, max_m0=16, n_threads=4)
    t.save('HNSW.n16_16_128_100w_L2')