import React, { useState, useEffect, useCallback } from "react";
import axios from "axios";
import { Container, Typography, Table, TableBody, TableCell, TableHead, TableRow, Paper } from "@mui/material";
import getBaseUrl from "./utils/getBaseUrl";

const BASE = getBaseUrl();
function TransactionHistory() {
  const [transactions, setTransactions] = useState([]);
  const accountId = localStorage.getItem("accountId");

  const fetchTransactions = useCallback(async () => {
    try {
      const res = await axios.get(`${BASE}/transactions/${accountId}`);
      // Sort transactions in descending order by timestamp
      const sortedTxns = res.data.sort((a, b) => b.timestamp - a.timestamp);
      setTransactions(sortedTxns);
    } catch (err) {
      console.error("Error fetching transactions", err);
    }
  }, [accountId]);

  useEffect(() => {
    fetchTransactions();
  }, [fetchTransactions]);

  return (
    <Container maxWidth="md" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Transaction History</Typography>
      {transactions.length === 0 ? (
        <Typography>No transactions found.</Typography>
      ) : (
        <Paper>
          <Table>
            <TableHead>
              <TableRow>
                <TableCell>ID</TableCell>
                <TableCell>Type</TableCell>
                <TableCell>Amount</TableCell>
                <TableCell>Timestamp</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {transactions.map((txn) => (
                <TableRow key={txn.transaction_id}>
                  <TableCell>{txn.transaction_id}</TableCell>
                  <TableCell>{txn.transaction_type}</TableCell>
                  <TableCell>{txn.amount}</TableCell>
                  <TableCell>{new Date(txn.timestamp * 1000).toLocaleString()}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        </Paper>
      )}
    </Container>
  );
}

export default TransactionHistory;
