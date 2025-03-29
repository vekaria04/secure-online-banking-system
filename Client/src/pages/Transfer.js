import React, { useState, useEffect, useCallback } from "react";
import axios from "axios";
import { Container, Typography, TextField, Button, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";

const BASE = process.env.REACT_APP_API_BASE_URL || "";

function Transfer() {
  const [balance, setBalance] = useState(0);
  const [amount, setAmount] = useState("");
  const [receiverEmail, setReceiverEmail] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();
  const accountId = localStorage.getItem("accountId");


  const fetchBalance = useCallback(async () => {
    try {
      const res = await axios.get(`${BASE}/balance/${accountId}`);
      setBalance(res.data.balance);
    } catch (err) {
      console.error("Failed to fetch balance", err);
    }
  }, [accountId]);


  useEffect(() => {
    fetchBalance();
  }, [fetchBalance]);


  async function handleSubmit(e) {
    e.preventDefault();
    setMessage("");
    try {
      const token = localStorage.getItem("token");
      await axios.post(`${BASE}/transaction`,
        {
          type: "Transfer",
          accountId: parseInt(accountId),
          amount: parseFloat(amount),
          receiverEmail: receiverEmail
        },
        {
          headers: {
            Authorization: `Bearer ${token}`
          }
        }
      );
      setMessage("Transfer successful");
      fetchBalance();
      navigate("/dashboard");
    } catch (err) {
      setMessage("Transfer failed");
    }
  }


  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Transfer Funds</Typography>
      <Typography variant="h6" gutterBottom>Your Balance: ${balance}</Typography>
      {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
      <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField label="Amount" type="number" value={amount} onChange={(e) => setAmount(e.target.value)} required />
        <TextField label="Receiver Email" type="email" value={receiverEmail} onChange={(e) => setReceiverEmail(e.target.value)} required />
        <Button variant="contained" type="submit">Submit Transfer</Button>
        <Button variant="outlined" onClick={() => navigate("/dashboard")}>Back to Dashboard</Button>
      </Box>
    </Container>
  );
}


export default Transfer;
